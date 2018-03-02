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


#include "circlefit.h"

/*!
    \headerfile <circlefit.h>
    \title Circle Fit Algorithms.
    \brief The <circlefit.h> header file provides various methods to fit circle.
 */

#include <QImage>
#include <QPoint>
#include <QVector>
#include <QColor>
#include "global.h"

namespace MEMS {

/*!
    \struct CircleData
    \brief The CircleData structure contains the center and radius of a circle.


    \variable CircleData::center

    The center of the circle.

    \variable CircleData::radius

    The radius of the circle.
 */

/*!
    \typedef CircleFitFunction
    This is a typedef for a pointer to a function with the following signature:

    \code
        CircleData myCircleFit(const QVector<QPoint>&)
    \endcode
 */


/*!
    Get positions of the white pixels in the \a monochrome image.

    \note The format of the input image should
    be \c QImage::Format_Mono or \c QImage::Format_MonoLSB.
 */
QVector<QPoint> whitePixelPositions(const QImage& monochrome)
{
    Q_ASSERT_X(monochrome.format()==QImage::Format_Mono
               || monochrome.format()==QImage::Format_MonoLSB,
               __func__,"Only monochrome image is valid.");
    const int whiteIndex = monochrome.colorTable().first() == QColor(Qt::white).rgba() ? 0 : 1;

    QVector<QPoint> result;
    const int width = monochrome.width();
    const int height = monochrome.height();
    switch (monochrome.format())
    {
    case QImage::Format_Mono:
        for (int y=0; y<height; ++y)
        {
            const uchar* line = monochrome.constScanLine(y);
            for (int x=0; x<width; ++x)
            {
                if (((line[x>>3] >> (7-(x&7))) & 1) == whiteIndex)
                {
                    result.append({x,y});
                }
            }
        }
        break;
    case QImage::Format_MonoLSB:
        for (int y=0; y<height; ++y)
        {
            const uchar* line = monochrome.constScanLine(y);
            for (int x=0; x<width; ++x)
            {
                if (((line[x>>3] >> (x&7)) & 1) == whiteIndex)
                {
                    result.append({x,y});
                }
            }
        }
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    return result;
}

/*!
    Naïve !
 */
CircleData naiveCircleFit(const QVector<QPoint>& points)
{
    const int num = points.size();
    CircleData circle;

    circle.center = ::std::accumulate(points.cbegin(),points.cend(),QPointF{0,0})/num;

    qreal sumR2 = 0;
    for (const auto& point : points)
    {
        QPointF vec = point-circle.center;
        sumR2 += vec.x()*vec.x() + vec.y()*vec.y();
    }
    circle.radius = ::std::sqrt(sumR2/num);

    return circle;
}

/*!
    a linear least square method to find the circle by minimizing the function

       f(x_c,y_c,R) = Σ((x_i - x_c)^2 + (y_i - y_c)^2 - R^2)^2

    where ((x_i - x_c)^2 + (y_i - y_c)^2 - R^2)^2 is so called algebraic distance
    from the point (x_i, y_i) to the circle.
 */
CircleData simpleAlgebraicCircleFit(const QVector<QPoint>& points)
{
    const int num = points.size();
    if (num<3)
    {
        qWarning() << __func__ << ": Fitting a circle requires at least three points";
        return naiveCircleFit(points);
    }

    CircleData circle;
    qreal sx1=0, sy1=0;
    qreal sx2=0, sy2=0;
    qreal sx3=0, sy3=0;
    qreal sx2y1=0, sx1y2=0;
    qreal sx1y1=0;
    for (const auto& point : points)
    {
        qreal x = point.x();
        qreal y = point.y();
        qreal x2 = x*x;
        qreal y2 = y*y;
        sx1 += x;       sy1 += y;
        sx2 += x2;      sy2 += y2;
        sx3 += x*x2;    sy3 += y*y2;
        sx2y1 += x2*y;  sx1y2 += x*y2;
        sx1y1 += x*y;
    }

    qreal px2 = num * sx2 - sx1 * sx1;
    qreal py2 = num * sy2 - sy1 * sy1;
    qreal pxy = num * sx1y1 - sx1 * sy1;
    qreal px1y2 = num * (sx3 + sx1y2) - (sx2 + sy2) * sx1;
    qreal px2y1 = num * (sx2y1 + sy3) - (sx2 + sy2) * sy1;

    qreal ca = (px1y2 * py2 - px2y1 * pxy) / (pxy * pxy - px2 * py2);
    qreal cb = (px2y1 * px2 - px1y2 * pxy) / (pxy * pxy - px2 * py2);
    qreal cc = - (ca * sx1 + cb * sy1 + sx2 + sy2) / num;

    circle.center = {ca/(-2), cb/(-2)};
    circle.radius = ::std::sqrt(ca*ca + cb*cb - 4*cc)/2;
    return circle;
}

/*!
    This is an algebraic fit based on the article

    \quotation
    A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
    Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)
    \endquotation

    This method combines the Pratt and Taubin fits to eliminate the essential bias.
 */
CircleData hyperAlgebraicCircleFit(const QVector<QPoint>& points)
{
    using ::std::abs;

    const int num = points.size();
    if (num<3)
    {
        qWarning() << __func__ << ": Fitting a circle requires at least three points";
        return naiveCircleFit(points);
    }

    CircleData circle;

    const QPointF mean = ::std::accumulate(points.cbegin(),points.cend(),QPointF{0,0})/num;

    // calculate moments
    qreal mxx=0, myy=0, mxy=0;
    qreal mxz=0, myz=0, mzz=0;
    for (const auto& point : points)
    {
        qreal x = point.x() - mean.x();
        qreal y = point.y() - mean.y();
        qreal z = x*x + y*y;
        mxx += x*x; myy += y*y; mxy += x*y;
        mxz += x*z; myz += y*z; mzz += z*z;
    }
    mxx /= num; myy /= num; mxy /= num;
    mxz /= num; myz /= num; mzz /= num;
    qreal mz = mxx + myy;
    qreal covxy = mxx*myy - mxy*mxy;
    qreal varz = mzz - mz*mz;

    // computing the coefficients of the characteristic polynomial
    qreal a2 = 4*covxy - 3*mz*mz - mzz;
    qreal a1 = varz*mz + 4*covxy*mz - mxz*mxz - myz*myz;
    qreal a0 = mxz*(mxz*myy - myz*mxy) + myz*(myz*mxx - mxz*mxy) - varz*covxy;
    qreal a22 = 2*a2;

    // finding the root of the characteristic polynomial
    qreal t=0, f=a0;
    constexpr uint maxIter = 99;
    for (uint iter=0; iter<maxIter; ++iter)
    {
        qreal df = a1 + t*(a22 + 16*t*t);
        qreal t_ = t - f/df;
        if (qFuzzyIsNull(t-t_) || !::std::isfinite(t_))
            break;
        qreal f_ = a0 + t_*(a1 + t_*(a2 + 4*t_*t_));
        if (abs(f_) >= abs(f))
            break;
        t = t_; f = f_;
    }

    qreal det = t*t - t*mz + covxy;
    qreal xCenter = (mxz*(myy - t) - myz*mxy) / det / 2;
    qreal yCenter = (myz*(mxx - t) - mxz*mxy) / det / 2;

    circle.center = mean + QPointF{xCenter, yCenter};
    circle.radius = ::std::sqrt(xCenter*xCenter + yCenter*yCenter + mz - 2*t);
    return circle;
}

/*!
    \internal

    Calculate geometric distance between the \a point and the \a circle.
 */
static inline qreal geometricError(CircleData circle, const QPoint& point)
{
    using ::std::abs;
    using ::std::hypot;
    return abs(hypot(point.x()-circle.center.x(),point.y()-circle.center.y())-circle.radius);
}

// do nothing
CircleData noCorrection(CircleFitFunction fit, const QVector<QPoint>& points)
{
    return fit(points);
}

/*!
    Using the median error as threshold to eliminate the error points.
    Reduce errors by iteration.
 */
CircleData medianErrorCorrection(CircleFitFunction fit, const QVector<QPoint>& points)
{
    using size_type = typename QVector<QPoint>::size_type;
    constexpr uint maxIter = 99;
    constexpr qreal proposalError = 4.5;
    const size_type size = points.size();
    const size_type medianIndex = size/2;

    CircleData circle = fit(points);
    QVector<qreal> errors(size,0);
    QVector<QPoint> validPoints = points;
    qreal lastMedianError = ::std::numeric_limits<qreal>::max();
    for (uint iter=0; iter<maxIter; ++iter)
    {
        // get median error
        ::std::transform(points.begin(),points.end(),errors.begin(),
                         [&circle](const QPoint& point){
            return geometricError(circle,point);
        });
        ::std::nth_element(errors.begin(),errors.begin()+medianIndex,errors.end());
        qreal medianError = errors.at(medianIndex);

        // check
        if (medianError < proposalError)
            return circle; // accept
        if (qFuzzyIsNull(medianError-lastMedianError))
            break; // not converge
        lastMedianError = medianError;

        // fit circle
        validPoints.clear();
        ::std::copy_if(points.begin(),points.end(),::std::back_inserter(validPoints),
                       [&circle,medianError](const QPoint& point){
            return geometricError(circle,point) < medianError;
        });
        circle = fit(validPoints);

        MAYBE_INTERRUPT();
    }
    qWarning() << __func__ << ": Unable to converge to the proposal error";
    return circle;
}

/*!
    Select points to fit based on the connectivity.
 */
CircleData connectivityBasedCorrection(CircleFitFunction fit, const QVector<QPoint>& points)
{
    static const auto isNeighborhood = [](const QPoint& a, const QPoint& b)->bool{
        using ::std::abs;
        constexpr int eps = 1;
        return abs(a.x()-b.x())<=eps && abs(a.y()-b.y())<=eps;
    };

    using size_type = typename QVector<QPoint>::size_type;

    QVector<QPoint> unclassified = points;
    QVector<QPoint> candidate;
    QVector<QPoint> toFind;
    CircleData circle = fit(points);
    qreal minOfMaxError = ::std::numeric_limits<qreal>::max();
    size_type maxSize = 0;
    while (!unclassified.isEmpty())
    {
        // to find candidate
        QPoint p = unclassified.takeLast();
        candidate.append(p);
        toFind.append(p);
        while (!toFind.isEmpty())
        {
            QPoint p = toFind.takeFirst(); // queue like
            for (const auto& point : qAsConst(unclassified))
            {
                if (isNeighborhood(p,point))
                {
                    toFind.append(point);
                }
            }
            for (const auto& finded : qAsConst(toFind))
            {
                candidate.append(finded);
                unclassified.removeOne(finded);
            }
        }

        // check candidate
        CircleData tmpCircle = fit(candidate);
        if (candidate.size() > maxSize)
        {
            maxSize = candidate.size();
            QVector<qreal> errors;
            ::std::transform(candidate.cbegin(),candidate.cend(),::std::back_inserter(errors),
                             [&tmpCircle](const QPoint& point){
                return geometricError(tmpCircle,point);
            });
            qreal maxError = *::std::max_element(errors.cbegin(),errors.cend())/tmpCircle.radius;
            if (maxError < minOfMaxError)
            {
                minOfMaxError = maxError;
                circle = tmpCircle; // accept
            }
        }
        candidate.clear();

        MAYBE_INTERRUPT();
    }
    return circle;
}

} // namespace MEMS
