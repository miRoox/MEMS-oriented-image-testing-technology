#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>

namespace Ui {
class MainPanel;
}

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);
    ~MainPanel();

private slots:
    void on_horizontalSliderGS_valueChanged(int value);
    void on_doubleSpinBoxGS_valueChanged(double arg1);

private:
    Ui::MainPanel *ui;
};

#endif // MAINPANEL_H
