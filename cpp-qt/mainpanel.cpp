#include "mainpanel.h"
#include "ui_mainpanel.h"  // generate from mainpanel.ui
#include <QImage>
#include <QIcon>
#include <QtDebug>

static constexpr const char DefaultOriginKey[] = "A";

static inline QString keyToImagePath(const QString& key)
{
    return ":/images/" + key + ".bmp";
}

MainPanel::MainPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPanel)
{
    ui->setupUi(this);

    setOrigin(DefaultOriginKey);
}

MainPanel::~MainPanel()
{
    delete ui;
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
