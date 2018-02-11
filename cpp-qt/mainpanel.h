#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QThread>
#include <QPointer>
#include <QString>
#include <QPointF>
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
    void originImageChange(const QImage& origin);

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

private:
    Ui::MainPanel *ui;
    QThread workerThread;
    QPointer<Processor> processor;
    QString currentOriginKey;
    QPointF center;
    qreal radius;
};

#endif // MAINPANEL_H
