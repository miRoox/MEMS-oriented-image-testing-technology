/**
 ** MIT License
 **
 ** This file is part of the MEMS-oriented-image-testing-technology project.
 ** Copyright (c) 2018 Lu <miroox@outlook.com>.
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a copy
 ** of this software and associated documentation files (the "Software"), to deal
 ** in the Software without restriction, including without limitation the rights
 ** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 ** copies of the Software, and to permit persons to whom the Software is
 ** furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included in all
 ** copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
 ** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 ** SOFTWARE.
 **/


#include "mainpanel.h"
#include "ui_mainpanel.h"  // generate from mainpanel.ui
#include "processor.h"
#include "progressupdater.h"
#include <cmath>
#include <QImage>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QStringList>
#include <QFileDialog>
#include <QCursor>
#include <QApplication>
#include <QtDebug>

static constexpr const char DefaultOriginKey[] = "A";

static inline QString keyToImagePath(const QString& key)
{
    return ":/images/" + key + ".bmp";
}

MainPanel::MainPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPanel),
    center(0,0),radius(0),
    MapFilterMethod{
        {tr("Box filter"), Configuration::BoxFilter},
        {tr("Gaussian filter"), Configuration::GaussianFilter},
        {tr("Median filter"), Configuration::MedianFilter},
        {tr("Mean shift filter"), Configuration::MeanShiftFilter}
    },
    MapThresMethod{
        {tr("Otsu's threshold clustering algorithm"), Configuration::Cluster},
        {tr("Mean of gray levels"), Configuration::Mean},
        {tr("Moment-preserving thresholding method"), Configuration::Moments},
        {tr("Huang's fuzzy thresholding method"), Configuration::Fuzziness},
        {tr("P-tile thresholding"), Configuration::PTile}
    },
    MapEdgeMethod{
        {tr("Sobel operator"), Configuration::Sobel},
        {tr("Prewitt operator"), Configuration::Prewitt},
        {tr("Scharr operator"), Configuration::Scharr},
        {tr("Laplacian operator"), Configuration::Laplacian}
    },
    MapFitMethod{
        {tr("Naive fit"), Configuration::NaiveFit},
        {tr("Simple algebraic fit"), Configuration::SimpleAlgebraicFit},
        {tr("Hyper algebraic fit"), Configuration::HyperAlgebraicFit}
    },
    MapErrCorrMethod{
        {tr("No correction"), Configuration::NoCorrection},
        {tr("Median error correction"), Configuration::MedianError},
        {tr("Connectivity-based correction"), Configuration::ConnectivityBased}
    }
{
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->progressLabel->hide();
    // init combo box
    ui->comboBoxFilter->addItems(MapFilterMethod.keys());
    ui->comboBoxThres->addItems(MapThresMethod.keys());
    ui->comboBoxEdge->addItems(MapEdgeMethod.keys());
    ui->comboBoxFit->addItems(MapFitMethod.keys());
    ui->comboBoxCorr->addItems(MapErrCorrMethod.keys());
    // init ui connection
    connect(ui->spinBoxFR,qOverload<int>(&QSpinBox::valueChanged),
            ui->spinBoxFRG,&QSpinBox::setValue);
    connect(ui->spinBoxFRG,qOverload<int>(&QSpinBox::valueChanged),
            ui->spinBoxFR,&QSpinBox::setValue);
    connect(ui->spinBoxFR,qOverload<int>(&QSpinBox::valueChanged),
            ui->spinBoxFRMS,&QSpinBox::setValue);
    connect(ui->spinBoxFRMS,qOverload<int>(&QSpinBox::valueChanged),
            ui->spinBoxFR,&QSpinBox::setValue);
    connect(ui->spinBoxFR,qOverload<int>(&QSpinBox::valueChanged),
            this,&MainPanel::changeFilterRadiusRequest);
    connect(ui->doubleSpinBoxGS,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,&MainPanel::changeGaussianSigmaRequest);
    connect(ui->doubleSpinBoxCRMS,qOverload<double>(&QDoubleSpinBox::valueChanged),
            this,&MainPanel::changeColorRadiusRequest);
    connect(ui->spinBoxMLMS,qOverload<int>(&QSpinBox::valueChanged),
            this,&MainPanel::changeMaxLevelRequest);

    // init progress
    progressUpdater = new ProgressUpdater;
    progressUpdater->moveToThread(&workerThread);
    connect(&workerThread,&QThread::finished,
            progressUpdater,&ProgressUpdater::deleteLater);

    // recieve from progressUpdater
    connect(progressUpdater,&ProgressUpdater::beginRequest,
            ui->progressBar,&QProgressBar::show);
    connect(progressUpdater,&ProgressUpdater::beginRequest,
            ui->progressLabel,&QProgressBar::show);
    connect(progressUpdater,&ProgressUpdater::beginRequest,
            this,&MainPanel::overrideBusyCursor);
    connect(progressUpdater,&ProgressUpdater::endRequest,
            ui->progressBar,&QProgressBar::hide);
    connect(progressUpdater,&ProgressUpdater::endRequest,
            ui->progressLabel,&QProgressBar::hide);
    connect(progressUpdater,&ProgressUpdater::endRequest,
            ui->progressBar,&QProgressBar::reset);
    connect(progressUpdater,&ProgressUpdater::endRequest,
            this,&MainPanel::restoreCursor);
    connect(progressUpdater,&ProgressUpdater::valueChange,
            ui->progressBar,&QProgressBar::setValue);
    connect(progressUpdater,&ProgressUpdater::textTipChange,
            ui->progressLabel,&QLabel::setText);

    // update view
    connect(ui->thumbnail1,&ThumbnailView::viewChanged,
            this,&MainPanel::updateView);
    connect(ui->thumbnail2,&ThumbnailView::viewChanged,
            this,&MainPanel::updateView);
    connect(ui->thumbnail3,&ThumbnailView::viewChanged,
            this,&MainPanel::updateView);
    connect(ui->thumbnail4,&ThumbnailView::viewChanged,
            this,&MainPanel::updateView);

    // load config
    auto config = loadConfigs(DefaultOriginKey);
    setByConfig(config);
    // init processor
    processor = new Processor(config);
    processor->moveToThread(&workerThread);
    connect(&workerThread,&QThread::finished,
            processor,&Processor::deleteLater);
    connect(&workerThread,&QThread::started,
            this,&MainPanel::initializeOnRun);

    // send request to Processor
    connect(this,&MainPanel::changeOriginRequest,
            processor,&Processor::setOriginImage);
    connect(this,&MainPanel::setConfigurationsRequset,
            processor,&Processor::setConfigurations);
    connect(this,&MainPanel::changeFilterMethodRequest,
            processor,&Processor::setFilterMethod);
    connect(this,&MainPanel::changeThresholdingMethodRequest,
            processor,&Processor::setThresholdingMethod);
    connect(this,&MainPanel::changeEdgeDetectionMethodRequest,
            processor,&Processor::setEdgeDetectionMethod);
    connect(this,&MainPanel::changeCircleFitMethodRequest,
            processor,&Processor::setCircleFitMethod);
    connect(this,&MainPanel::changeErrorCorrectionMethodRequest,
            processor,&Processor::setErrorCorrectionMethod);
    connect(this,&MainPanel::changeFilterRadiusRequest,
            processor,&Processor::setFilterRadius);
    connect(this,&MainPanel::changeGaussianSigmaRequest,
            processor,&Processor::setGaussianSigma);
    connect(this,&MainPanel::changeColorRadiusRequest,
            processor,&Processor::setColorRadius);
    connect(this,&MainPanel::changeMaxLevelRequest,
            processor,&Processor::setMaxLevel);
    connect(this,&MainPanel::changePTileValueRequest,
            processor,&Processor::setPTileValue);
    connect(this,&MainPanel::saveConfigurationsRequest,
            processor,&Processor::saveConfigurations);

    // recieve data from Processor
    connect(processor,&Processor::filteredImageChanged,
            this,&MainPanel::setFilteredImage);
    connect(processor,&Processor::binaryImageChanged,
            this,&MainPanel::setBinaryImage);
    connect(processor,&Processor::edgeImageChanged,
            this,&MainPanel::setEdgeImage);
    connect(processor,&Processor::circleImageChanged,
            this,&MainPanel::setFitImage);
    connect(processor,&Processor::circleCenterChanged,
            this,&MainPanel::setCircleCenter);
    connect(processor,&Processor::circleRadiusChanged,
            this,&MainPanel::setCircleRadius);

    updateCircle();

    workerThread.start();
}

MainPanel::~MainPanel()
{
    workerThread.requestInterruption();
    workerThread.quit();
    bool quited = workerThread.wait();
    Q_ASSERT(quited);
    delete ui;
}

void MainPanel::setByConfig(const Configuration& config)
{
    emit setConfigurationsRequset(config);
    ui->comboBoxFilter->setCurrentText(MapFilterMethod.key(config.filterMethod()));
    ui->comboBoxThres->setCurrentText(MapThresMethod.key(config.thresholdingMethod()));
    ui->comboBoxEdge->setCurrentText(MapEdgeMethod.key(config.edgeDetectionMethod()));
    ui->comboBoxFit->setCurrentText(MapFitMethod.key(config.circleFitMethod()));
    ui->comboBoxCorr->setCurrentText(MapErrCorrMethod.key(config.errorCorrectionMethod()));
    ui->spinBoxFR->setValue(config.filterRadius());
    ui->doubleSpinBoxGS->setValue(config.gaussianSigma());
    ui->doubleSpinBoxCRMS->setValue(config.colorRadius());
    ui->spinBoxMLMS->setValue(config.maxLevel());
    ui->spinBoxPT->setValue(::std::round(100*config.pTileValue()));
}

void MainPanel::setOrigin(const QString& key)
{
    if (key == currentOriginKey)
        return;
    currentOriginKey = key;
    if (key == "A")
    {
        ui->radioButtonA->setChecked(true);
        setWindowTitle(tr("A"));
    }
    else if (key == "B")
    {
        ui->radioButtonB->setChecked(true);
        setWindowTitle(tr("B"));
    }
    else // key == "C"
    {
        Q_ASSERT(key == "C");
        ui->radioButtonC->setChecked(true);
        setWindowTitle(tr("C"));
    }
    setWindowIcon(QIcon(keyToImagePath(key)));
    emit changeOriginRequest(QImage(keyToImagePath(key)));
}

void MainPanel::updateCircle()
{
    ui->labelResult->setText(tr("The center of the circle is (%1, %2), and the radius is %3")
                             .arg(center.x()).arg(center.y()).arg(radius));
}

void MainPanel::initializeOnRun()
{
    setOrigin(DefaultOriginKey);
}

void MainPanel::setFilteredImage(const QImage& img)
{
    ui->thumbnail1->setOriginImage(img);
}

void MainPanel::setBinaryImage(const QImage& img)
{
    ui->thumbnail2->setOriginImage(img);
}

void MainPanel::setEdgeImage(const QImage& img)
{
    ui->thumbnail3->setOriginImage(img);
}

void MainPanel::setFitImage(const QImage& img)
{
    ui->thumbnail4->setOriginImage(img);
}

void MainPanel::setCircleCenter(const QPointF& center)
{
    this->center = center;
    updateCircle();
}

void MainPanel::setCircleRadius(qreal radius)
{
    this->radius = radius;
    updateCircle();
}

void MainPanel::overrideBusyCursor()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
}

void MainPanel::restoreCursor()
{
    QApplication::restoreOverrideCursor();
}

void MainPanel::updateView(bool isOrigin)
{
    if (isOrigin)
    {
        ThumbnailView* current = q_check_ptr(qobject_cast<ThumbnailView*>(sender()));
        ui->thumbnail1->setVisible(current == ui->thumbnail1);
        ui->labelText1->setVisible(current == ui->thumbnail1);
        ui->thumbnail2->setVisible(current == ui->thumbnail2);
        ui->labelText2->setVisible(current == ui->thumbnail2);
        ui->thumbnail3->setVisible(current == ui->thumbnail3);
        ui->labelText3->setVisible(current == ui->thumbnail3);
        ui->thumbnail4->setVisible(current == ui->thumbnail4);
        ui->labelText4->setVisible(current == ui->thumbnail4);
    }
    else
    {
        ui->thumbnail1->show();
        ui->labelText1->show();
        ui->thumbnail2->show();
        ui->labelText2->show();
        ui->thumbnail3->show();
        ui->labelText3->show();
        ui->thumbnail4->show();
        ui->labelText4->show();
    }
}

void MainPanel::on_horizontalSliderGS_valueChanged(int value)
{
    ui->doubleSpinBoxGS->setValue(value/10.);
}

void MainPanel::on_doubleSpinBoxGS_valueChanged(double arg1)
{
    ui->horizontalSliderGS->setValue(static_cast<int>(10*arg1));
}

void MainPanel::on_horizontalSliderCRMS_valueChanged(int value)
{
    ui->doubleSpinBoxCRMS->setValue(value/100.);
}

void MainPanel::on_doubleSpinBoxCRMS_valueChanged(double arg1)
{
    ui->horizontalSliderCRMS->setValue(static_cast<int>(100*arg1));
}

void MainPanel::on_radioButtonA_toggled(bool checked)
{
    if (checked)
    {
        setOrigin("A");
    }
}

void MainPanel::on_radioButtonB_toggled(bool checked)
{
    if (checked)
    {
        setOrigin("B");
    }
}

void MainPanel::on_radioButtonC_toggled(bool checked)
{
    if (checked)
    {
        setOrigin("C");
    }
}

void MainPanel::on_comboBoxFilter_currentIndexChanged(const QString& arg1)
{
    auto method = MapFilterMethod.value(arg1,Configuration::defaultFilterMethod());
    emit changeFilterMethodRequest(method);
    switch (method)
    {
    case Configuration::GaussianFilter:
        ui->stackedWidgetFilter->setCurrentWidget(ui->pageFilterG);
        break;
    case Configuration::MeanShiftFilter:
        ui->stackedWidgetFilter->setCurrentWidget(ui->pageFilterMS);
        break;
    default:
        ui->stackedWidgetFilter->setCurrentWidget(ui->pageFilter);
        break;
    }
}

void MainPanel::on_comboBoxThres_currentIndexChanged(const QString& arg1)
{
    auto method = MapThresMethod.value(arg1,Configuration::defaultThresholdingMethod());
    emit changeThresholdingMethodRequest(method);
    switch (method)
    {
    case Configuration::PTile:
        ui->stackedWidgetThres->setCurrentWidget(ui->pageThresPT);
        break;
    default:
        ui->stackedWidgetThres->setCurrentWidget(ui->pageThres);
        break;
    }
}

void MainPanel::on_comboBoxEdge_currentIndexChanged(const QString& arg1)
{
    emit changeEdgeDetectionMethodRequest(MapEdgeMethod.value(arg1,Configuration::defaultEdgeDetectionMethod()));
}

void MainPanel::on_comboBoxFit_currentIndexChanged(const QString& arg1)
{
    emit changeCircleFitMethodRequest(MapFitMethod.value(arg1,Configuration::defaultCircleFitMethod()));
}

void MainPanel::on_comboBoxCorr_currentIndexChanged(const QString& arg1)
{
    emit changeErrorCorrectionMethodRequest(MapErrCorrMethod.value(arg1,Configuration::defaultErrorCorrectionMethod()));
}

void MainPanel::on_spinBoxPT_valueChanged(int arg1)
{
    emit changePTileValueRequest(arg1/100.);
}

void MainPanel::on_pushButtonSaveConfig_clicked()
{
    emit saveConfigurationsRequest(currentOriginKey);
}

void MainPanel::on_pushButtonLoadConfig_clicked()
{
    Configuration config = loadConfigs(currentOriginKey);
    setByConfig(config);
    qInfo() << "For" << currentOriginKey << ", load the" << config;
}
