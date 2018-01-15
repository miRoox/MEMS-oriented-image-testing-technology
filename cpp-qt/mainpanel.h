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

private:
    Ui::MainPanel *ui;
};

#endif // MAINPANEL_H
