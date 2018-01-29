#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <QVector>
#include <QImage>

namespace MEMS {

using MatrixKernel = QVector<QVector<qreal>>;

enum class PaddingType
{
    None = 0,
    Fixed = 1,
    Periodic = 2,
    Reflected = 3,
};


QRgb getPixel(const QImage& img, int x, int y, PaddingType padding);
QRgb getPixel(const QImage& img, const QPoint& position, PaddingType padding);
QRgb getPixel(const QImage& img, int x, int y, QRgb padding);
QRgb getPixel(const QImage& img, const QPoint& position, QRgb padding);
QColor getPixelColor(const QImage& img, int x, int y, PaddingType padding);
QColor getPixelColor(const QImage& img, const QPoint& position, PaddingType padding);
QColor getPixelColor(const QImage& img, int x, int y, const QColor& padding);
QColor getPixelColor(const QImage& img, const QPoint& position, const QColor& padding);

//QImage convolve(const QImage& origin, const MatrixKernel& kernel, PaddingType padding);

} // namespace MEMS

#endif // IMAGEFILTER_H
