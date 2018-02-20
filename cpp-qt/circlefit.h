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


#ifndef CIRCLEFIT_H
#define CIRCLEFIT_H

#include <QPointF>

class QImage;
class QPoint;
template<typename T>
class QVector;

namespace MEMS {

struct CircleData
{
    Q_DECL_CONSTEXPR CircleData() : center(0,0),radius(0) {}
    friend Q_DECL_CONSTEXPR inline bool operator==(const CircleData&, const CircleData&);

    QPointF center;
    qreal radius;
};

Q_DECL_CONSTEXPR inline bool operator==(const CircleData& a, const CircleData& b)
{
    return a.center==b.center && qFuzzyIsNull(a.radius-b.radius);
}

extern QVector<QPoint> whitePixelPositions(const QImage& monochrome);

using CircleFitFunction = CircleData (*)(const QVector<QPoint>&);

// circle fit functions
extern CircleData naiveCircleFit(const QVector<QPoint>& points);
extern CircleData simpleAlgebraicCircleFit(const QVector<QPoint>& points);
extern CircleData hyperAlgebraicCircleFit(const QVector<QPoint>& points);

// error points correction functions
extern CircleData noCorrection(CircleFitFunction fit, const QVector<QPoint>& points);
extern CircleData medianErrorCorrection(CircleFitFunction fit, const QVector<QPoint>& points);
extern CircleData connectivityBasedCorrection(CircleFitFunction fit, const QVector<QPoint>& points);

} // namespace MEMS

#endif // CIRCLEFIT_H
