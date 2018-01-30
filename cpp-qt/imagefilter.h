#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <QVector>
#include <QImage>

namespace MEMS {

using MatrixKernel = QVector<QVector<qreal>>;

enum class PaddingType
{
//    None = 0,
    Fixed = 1,
    Periodic = 2,
    Reflected = 3,
};


extern QRgb getPixel(const QImage& img, int x, int y, PaddingType padding);
extern QRgb getPixel(const QImage& img, const QPoint& position, PaddingType padding);
extern QRgb getPixel(const QImage& img, int x, int y, QRgb padding);
extern QRgb getPixel(const QImage& img, const QPoint& position, QRgb padding);
extern QColor getPixelColor(const QImage& img, int x, int y, PaddingType padding);
extern QColor getPixelColor(const QImage& img, const QPoint& position, PaddingType padding);
extern QColor getPixelColor(const QImage& img, int x, int y, const QColor& padding);
extern QColor getPixelColor(const QImage& img, const QPoint& position, const QColor& padding);

extern QImage convolve(const QImage& origin, const MatrixKernel& kernel,
                       PaddingType padding = PaddingType::Fixed);
extern QImage convolve(const QImage& origin, const MatrixKernel& kernel, QRgb padding);
extern QImage convolve(const QImage& origin, const MatrixKernel& kernel, const QColor& padding);

} // namespace MEMS

#endif // IMAGEFILTER_H
