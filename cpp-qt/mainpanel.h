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
    void exportResultRequest(const QString& fileName);

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

private slots:
    void on_horizontalSliderGS_valueChanged(int value);
    void on_doubleSpinBoxGS_valueChanged(double arg1);
    void on_radioButtonA_toggled(bool checked);
    void on_radioButtonB_toggled(bool checked);
    void on_radioButtonC_toggled(bool checked);
    void on_comboBoxFilter_currentIndexChanged(const QString &arg1);
    void on_comboBoxThres_currentIndexChanged(const QString &arg1);
    void on_comboBoxEdge_currentIndexChanged(const QString &arg1);
    void on_comboBoxFit_currentIndexChanged(const QString &arg1);
    void on_pushButtonExport_clicked();

private:
    Ui::MainPanel *ui;
    QThread workerThread;
    QPointer<Processor> processor;

    QString currentOriginKey;
    QPointF center;
    qreal radius;

    const QMap<QString, Configuration::FilterMethod> MapFilterMethod;
    const QMap<QString, Configuration::ThresholdingMethod> MapThresMethod;
    const QMap<QString, Configuration::EdgeDetectionMethod> MapEdgeMethod;
    const QMap<QString, Configuration::CircleFitMethod> MapFitMethod;
};

#endif // MAINPANEL_H
