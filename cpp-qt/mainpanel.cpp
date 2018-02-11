#include "mainpanel.h"
#include "ui_mainpanel.h"  // generate from mainpanel.ui

MainPanel::MainPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPanel)
{
    ui->setupUi(this);
}

MainPanel::~MainPanel()
{
    delete ui;
}

void MainPanel::on_horizontalSliderGS_valueChanged(int value)
{
    ui->doubleSpinBoxGS->setValue(value/10.);
}

void MainPanel::on_doubleSpinBoxGS_valueChanged(double arg1)
{
    ui->horizontalSliderGS->setValue(static_cast<int>(10*arg1));
}
