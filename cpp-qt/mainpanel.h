#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QString>

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
    void setOrigin(const QString& key);

private slots:
    void on_horizontalSliderGS_valueChanged(int value);
    void on_doubleSpinBoxGS_valueChanged(double arg1);
    void on_radioButtonA_toggled(bool checked);
    void on_radioButtonB_toggled(bool checked);
    void on_radioButtonC_toggled(bool checked);

private:
    Ui::MainPanel *ui;
    QString currentOriginKey;
};

#endif // MAINPANEL_H
