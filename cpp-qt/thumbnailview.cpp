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


#include "thumbnailview.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QSize>
#include <QClipboard>
#include <QApplication>

static constexpr QSize snapshotSize{320,240};

ThumbnailView::ThumbnailView(QWidget *parent)
    : QWidget(parent), view(new QLabel)
{
    auto layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(view);
    setLayout(layout);
    setAttribute(Qt::WA_Hover);
    auto actionCopy = new QAction(tr("Copy"),this);
    auto actionSave = new QAction(tr("Save"),this);
    addAction(actionCopy);
    addAction(actionSave);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(actionCopy,&QAction::triggered,[this]{
        if (origin.isNull())
            return;
        QApplication::clipboard()->setPixmap(origin);
    });
    connect(actionSave,&QAction::triggered,
            this,&ThumbnailView::saveImage);
    setOriginPixmap( {} );
}

QPixmap ThumbnailView::originPixmap() const
{
    return origin;
}

void ThumbnailView::setOriginPixmap(const QPixmap& pixmap)
{
    origin = pixmap;
    if (pixmap.isNull())
    {
        view->setText(tr("<html><body><p style=\"color:gray\">No Image</p></body></html>"));
    }
    else
    {
        view->setPixmap(origin.scaled(snapshotSize,Qt::KeepAspectRatio));
    }
}

void ThumbnailView::setOriginImage(const QImage& image)
{
    return setOriginPixmap(QPixmap::fromImage(image));
}

void ThumbnailView::enterEvent(QEvent* e)
{
    if (!origin.isNull())
        view->setPixmap(origin);
    return QWidget::enterEvent(e);
}

void ThumbnailView::leaveEvent(QEvent* e)
{
    if (!origin.isNull())
        view->setPixmap(origin.scaled(snapshotSize,Qt::KeepAspectRatio));
    return QWidget::leaveEvent(e);
}

void ThumbnailView::saveImage()
{
    if (origin.isNull())
    {
        QMessageBox::information(this,tr("No image to save"),
                                 tr("There is no image to save, please retry later."),
                                 QMessageBox::Cancel);
        return;
    }
    QString key = window()->windowTitle();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Image"),
                                                    key + ".out.png",
                                                    tr("Images (*.png *.jpg *.jpeg *.bmp *.xpm)"));
    if (!fileName.isEmpty())
    {
        bool retry = false;
        do {
            if (origin.save(fileName))
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
