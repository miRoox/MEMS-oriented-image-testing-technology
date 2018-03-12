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


#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QWidget>
#include <QPixmap>

class QLabel;

class ThumbnailView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPixmap originPixmap READ originPixmap WRITE setOriginPixmap)
public:
    explicit ThumbnailView(QWidget *parent = nullptr);

    QPixmap originPixmap() const;

signals:
    void viewChanged(bool isOrigin);

public slots:
    void setOriginPixmap(const QPixmap& pixmap);
    void setOriginImage(const QImage& origin);

protected:
    virtual bool event(QEvent *event) override;

private slots:
    void saveImage();

private:
    QLabel* view;
    QPixmap origin;
    bool viewOrigin;
};

#endif // THUMBNAILVIEW_H
