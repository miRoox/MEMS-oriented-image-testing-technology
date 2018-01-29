#include "imagefilter.h"
#include <QImage>
#include <QPoint>

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

} // namespace MEMS
