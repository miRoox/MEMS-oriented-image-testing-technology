#include "snapshotview.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QSize>

static constexpr QSize snapshotSize{320,240};

SnapshotView::SnapshotView(QWidget *parent)
    : QWidget(parent), view(new QLabel)
{
    auto layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(view);
    setLayout(layout);
    setAttribute(Qt::WA_Hover);
    auto actionSave = new QAction(tr("Save"),this);
    addAction(actionSave);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(actionSave,&QAction::triggered,
            this,&SnapshotView::saveImage);
    setPixmap( {} );
}

QPixmap SnapshotView::pixmap() const
{
    return image;
}

QPixmap SnapshotView::snapshot() const
{
    const QPixmap* d = view->pixmap();
    if (!d)
        return {};
    return *d;
}

void SnapshotView::setPixmap(const QPixmap& pixmap)
{
    image = pixmap;
    if (pixmap.isNull())
    {
        view->setText(tr("<html><body><p style=\"color:gray\">No Image</p></body></html>"));
    }
    else
    {
        view->setPixmap(image.scaled(snapshotSize,Qt::KeepAspectRatio));
    }
}

void SnapshotView::setImage(const QImage& image)
{
    return setPixmap(QPixmap::fromImage(image));
}

void SnapshotView::enterEvent(QEvent* e)
{
    if (!image.isNull())
        view->setPixmap(image);
    return QWidget::enterEvent(e);
}

void SnapshotView::leaveEvent(QEvent* e)
{
    if (!image.isNull())
        view->setPixmap(image.scaled(snapshotSize,Qt::KeepAspectRatio));
    return QWidget::leaveEvent(e);
}

void SnapshotView::saveImage()
{
    QString key = window()->windowTitle();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Image"),
                                                    key + ".out.png",
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp *.xpm)"));
    if (!fileName.isEmpty())
    {
        bool retry = false;
        do {
            if (image.save(fileName))
            {
                QMessageBox::information(this,tr("Image saved sucessfully!"),
                                         tr("Image sucessfully saved to %1.").arg(fileName),
                                         QMessageBox::Ok);
                retry = false;
            }
            else
            {
                auto button =
                        QMessageBox::warning(this,tr("Image save failed!"),
                                             tr("Do you want to retry?"),
                                             QMessageBox::Retry,QMessageBox::Cancel);
                retry = button==QMessageBox::Retry;
            }
        }
        while(retry);
    }
}
