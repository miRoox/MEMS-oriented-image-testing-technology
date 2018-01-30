#include "imagefilter.h"
#include <QImage>
#include <QPoint>
#include <QColor>

#include <QtDebug>

namespace MEMS {

/*!
    \internal
 */
static QPoint calcPadding(const QImage& img, const QPoint& position, PaddingType padding)
{
    const int width = img.width();
    const int height = img.height();
    int x = position.x();
    int y = position.y();
    switch (padding)
    {
    case PaddingType::Fixed:
        if (x<0)
            x = 0;
        else if (x>=width)
            x = width-1;
        if (y<0)
            y = 0;
        else if (y>=height)
            y = height-1;
        break;
    case PaddingType::Periodic:
        x = x>=0 ? x%width
                 : x%width+width;
        y = y>=0 ? y%height
                 : y%height+height;
        break;
    case PaddingType::Reflected:
        while (x>=0 && x<width)
        {
            if (x<0)
                x = -x-1;
            else
                x = 2*width-x-1;
        }
        while (y>=0 && y<height)
        {
            if (y<0)
                y = -y-1;
            else
                y = 2*height-y-1;
        }
        break;
    default:
        break;
    }
    return QPoint(x,y);
}

QRgb getPixel(const QImage& img, int x, int y, PaddingType padding)
{
    return getPixel(img,QPoint(x,y),padding);
}

QRgb getPixel(const QImage& img, const QPoint& position, PaddingType padding)
{
    return img.pixel(calcPadding(img,position,padding));
}

QRgb getPixel(const QImage& img, int x, int y, QRgb padding)
{
    return img.valid(x,y) ? img.pixel(x,y) : padding;
}

QRgb getPixel(const QImage& img, const QPoint& position, QRgb padding)
{
    return img.valid(position) ? img.pixel(position) : padding;
}

QColor getPixelColor(const QImage& img, int x, int y, PaddingType padding)
{
    return getPixelColor(img,QPoint(x,y),padding);
}

QColor getPixelColor(const QImage& img, const QPoint& position, PaddingType padding)
{
    return img.pixelColor(calcPadding(img,position,padding));
}

QColor getPixelColor(const QImage& img, int x, int y, const QColor& padding)
{
    return img.valid(x,y) ? img.pixelColor(x,y) : padding;
}

QColor getPixelColor(const QImage& img, const QPoint& position, const QColor& padding)
{
    return img.valid(position) ? img.pixelColor(position) : padding;
}

QImage convolve(const QImage& origin, const MatrixKernel& kernel, PaddingType padding)
{
    QImage output(origin.size(),QImage::Format_RGB32);

    const int width = origin.width();
    const int height = origin.height();
    const int kerRows = kernel.size();
    const int kerCols = kernel.first().size();
    Q_ASSERT_X(kerRows%2==1,__func__,"Row of kernel must be odd");
    Q_ASSERT_X(kerCols%2==1,__func__,"Column of kernel must be odd");
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;
    for (const auto& row : kernel)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel is not a matrix");
    }

    for (int y=0; y<height; ++y)
    {
        for (int x=0; x<width; ++x)
        {
            qreal rr=0, gg=0, bb=0;
            for (int i=0; i<kerRows; ++i)
            {
                for (int j=0; j<kerCols; ++j)
                {
                    QRgb pix = getPixel(origin,
                                        x+i-kerCenterX,
                                        y+j-kerCenterY,
                                        padding);
                    auto kerVar = kernel.at(kerRows-1-i).at(kerCols-1-j);
                    rr += qRed(pix)*kerVar;
                    gg += qGreen(pix)*kerVar;
                    bb += qBlue(pix)*kerVar;
                }
            }
            int r=rr, g=gg, b=bb;
            r = qBound(0,r,0xff);
            g = qBound(0,g,0xff);
            b = qBound(0,b,0xff);
            output.setPixel(x,y,qRgb(r,g,b));
        }
    }

    return output.convertToFormat(origin.format());
}

} // namespace MEMS
