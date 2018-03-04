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


#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QThread>
#include <QPointer>
#include <QString>
#include <QPointF>
#include <QMap>
#include "configuration.h"

class Processor;
class ProgressUpdater;

namespace Ui {
class MainPanel;
}

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);
    ~MainPanel();

signals:
    void changeOriginRequest(const QImage& origin);
    void setConfigurationsRequset(const Configuration& config);
    void changeFilterMethodRequest(Configuration::FilterMethod method);
    void changeThresholdingMethodRequest(Configuration::ThresholdingMethod method);
    void changeEdgeDetectionMethodRequest(Configuration::EdgeDetectionMethod method);
    void changeCircleFitMethodRequest(Configuration::CircleFitMethod method);
    void changeErrorCorrectionMethodRequest(Configuration::ErrorCorrectionMethod method);
    void changeFilterRadiusRequest(uint radius);
    void changeGaussianSigmaRequest(qreal sigma);
    void changeColorRadiusRequest(qreal radius);
    void changeMaxLevelRequest(uint level);
    void changePTileValueRequest(qreal value);
    void saveConfigurationsRequest(const QString& group);

private:
    void setByConfig(const Configuration& config);
    void setOrigin(const QString& key);
    void updateCircle();

private slots:
    void initializeOnRun();
    void setFilteredImage(const QImage& img);
    void setBinaryImage(const QImage& img);
    void setEdgeImage(const QImage& img);
    void setFitImage(const QImage& img);
    void setCircleCenter(const QPointF& center);
    void setCircleRadius(qreal radius);
    void overrideBusyCursor();
    void restoreCursor();

private slots:
    void on_horizontalSliderGS_valueChanged(int value);
    void on_doubleSpinBoxGS_valueChanged(double arg1);
    void on_horizontalSliderCRMS_valueChanged(int value);
    void on_doubleSpinBoxCRMS_valueChanged(double arg1);
    void on_radioButtonA_toggled(bool checked);
    void on_radioButtonB_toggled(bool checked);
    void on_radioButtonC_toggled(bool checked);
    void on_comboBoxFilter_currentIndexChanged(const QString& arg1);
    void on_comboBoxThres_currentIndexChanged(const QString& arg1);
    void on_comboBoxEdge_currentIndexChanged(const QString& arg1);
    void on_comboBoxFit_currentIndexChanged(const QString& arg1);
    void on_comboBoxCorr_currentIndexChanged(const QString& arg1);
    void on_spinBoxPT_valueChanged(int arg1);
    void on_pushButtonSaveConfig_clicked();
    void on_pushButtonLoadConfig_clicked();

private:
    Ui::MainPanel* ui;
    QThread workerThread;
    QPointer<Processor> processor;
    ProgressUpdater* progressUpdater;
    Q_DISABLE_COPY(MainPanel)

    QString currentOriginKey;
    QPointF center;
    qreal radius;

    const QMap<QString, Configuration::FilterMethod> MapFilterMethod;
    const QMap<QString, Configuration::ThresholdingMethod> MapThresMethod;
    const QMap<QString, Configuration::EdgeDetectionMethod> MapEdgeMethod;
    const QMap<QString, Configuration::CircleFitMethod> MapFitMethod;
    const QMap<QString, Configuration::ErrorCorrectionMethod> MapErrCorrMethod;
};

#endif // MAINPANEL_H
