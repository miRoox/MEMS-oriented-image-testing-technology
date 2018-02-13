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

extern QImage convolve(const QImage& origin,
                       const MatrixKernel& kernel,
                       PaddingType padding = PaddingType::Fixed);
extern QImage convolve(const QImage& origin,
                       const MatrixKernel& kernel,
                       QRgb padding);
extern QImage convolve(const QImage& origin,
                       const MatrixKernel& kernel,
                       const QColor& padding);

extern QImage convolveXY(const QImage& origin,
                         const MatrixKernel& kerX,
                         const MatrixKernel& kerY,
                         PaddingType padding = PaddingType::Fixed);
extern QImage convolveXY(const QImage& origin,
                         const MatrixKernel& kerX,
                         const MatrixKernel& kerY,
                         QRgb padding);
extern QImage convolveXY(const QImage& origin,
                         const MatrixKernel& kerX,
                         const MatrixKernel& kerY,
                         const QColor& padding);

extern QImage boxFilter(const QImage& origin,
                        uint radius = 2,
                        PaddingType padding = PaddingType::Fixed);
extern QImage boxFilter(const QImage& origin,
                        uint radius,
                        QRgb padding);
extern QImage boxFilter(const QImage& origin,
                        uint radius,
                        const QColor& padding);

extern QImage gaussianFilter(const QImage& origin,
                             uint radius = 2,
                             PaddingType padding = PaddingType::Fixed);
extern QImage gaussianFilter(const QImage& origin,
                             uint radius,
                             qreal sigma,
                             PaddingType padding = PaddingType::Fixed);
extern QImage gaussianFilter(const QImage& origin,
                             uint radius,
                             QRgb padding);
extern QImage gaussianFilter(const QImage& origin,
                             uint radius,
                             qreal sigma,
                             QRgb padding);
extern QImage gaussianFilter(const QImage& origin,
                             uint radius,
                             const QColor& padding);
extern QImage gaussianFilter(const QImage& origin,
                             uint radius,
                             qreal sigma,
                             const QColor& padding);

extern QImage medianFilter(const QImage& origin, uint radius);

} // namespace MEMS

#endif // IMAGEFILTER_H
