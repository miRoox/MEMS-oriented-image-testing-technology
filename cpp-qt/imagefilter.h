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

extern QImage convolve(const QImage& origin, const MatrixKernel& kernel,
                       PaddingType padding = PaddingType::Fixed);
extern QImage convolve(const QImage& origin, const MatrixKernel& kernel, QRgb padding);
extern QImage convolve(const QImage& origin, const MatrixKernel& kernel, const QColor& padding);

extern QImage boxFilter(const QImage& origin, uint radius = 2,
                        PaddingType padding = PaddingType::Fixed);
extern QImage boxFilter(const QImage& origin, uint radius, QRgb padding);
extern QImage boxFilter(const QImage& origin, uint radius, const QColor& padding);

extern QImage gaussianFilter(const QImage& origin, uint radius = 2,
                             PaddingType padding = PaddingType::Fixed);
extern QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma,
                             PaddingType padding = PaddingType::Fixed);
extern QImage gaussianFilter(const QImage& origin, uint radius, QRgb padding);
extern QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma, QRgb padding);
extern QImage gaussianFilter(const QImage& origin, uint radius, const QColor& padding);
extern QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma, const QColor& padding);

} // namespace MEMS

#endif // IMAGEFILTER_H
