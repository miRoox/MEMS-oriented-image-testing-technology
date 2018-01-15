#include "mainpanel.h"
#include "ui_mainpanel.h"

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
