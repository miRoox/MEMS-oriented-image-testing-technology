#include "mainpanel.h"
#include "ui_mainpanel.h"  // generate from mainpanel.ui
#include "processor.h"
#include <QImage>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QtDebug>

static constexpr const char DefaultOriginKey[] = "A";

static inline QString keyToImagePath(const QString& key)
{
    return ":/images/" + key + ".bmp";
}

MainPanel::MainPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPanel),
    center(0,0),radius(0)
{
    ui->setupUi(this);

    auto config = loadConfigs(DefaultOriginKey);
    setByConfig(config);
    processor = new Processor(config);
    processor->moveToThread(&workerThread);
    connect(&workerThread,&QThread::started,
            this,&MainPanel::initializeOnRun);
    connect(this,&MainPanel::originImageChange,
            processor,&Processor::setOriginImage);
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
    workerThread.quit();
    bool quited = workerThread.wait();
    Q_ASSERT(quited);
    delete ui;
}

void MainPanel::setByConfig(const Configuration& config)
{
    Q_UNUSED(config)
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
    emit originImageChange(QImage(keyToImagePath(key)));
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

static constexpr QSize snapshotSize{320,240};

void MainPanel::setFilteredImage(const QImage& img)
{
    ui->labelImage1->setPixmap(QPixmap::fromImage(img.scaled(snapshotSize,Qt::KeepAspectRatio)));
}

void MainPanel::setBinaryImage(const QImage& img)
{
    ui->labelImage2->setPixmap(QPixmap::fromImage(img.scaled(snapshotSize,Qt::KeepAspectRatio)));
}

void MainPanel::setEdgeImage(const QImage& img)
{
    ui->labelImage3->setPixmap(QPixmap::fromImage(img.scaled(snapshotSize,Qt::KeepAspectRatio)));
}

void MainPanel::setFitImage(const QImage& img)
{
    ui->labelImage4->setPixmap(QPixmap::fromImage(img.scaled(snapshotSize,Qt::KeepAspectRatio)));
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

void MainPanel::on_horizontalSliderGS_valueChanged(int value)
{
    ui->doubleSpinBoxGS->setValue(value/10.);
}

void MainPanel::on_doubleSpinBoxGS_valueChanged(double arg1)
{
    ui->horizontalSliderGS->setValue(static_cast<int>(10*arg1));
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
