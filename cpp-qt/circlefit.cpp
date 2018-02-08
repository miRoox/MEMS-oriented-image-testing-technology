#include "circlefit.h"
#include <QImage>
#include <QPoint>
#include <QVector>
#include <QColor>
#include <cmath>
#include <QtDebug>

namespace MEMS {

QVector<QPoint> whitePixelPositions(const QImage& img)
{
    Q_ASSERT_X(img.format()==QImage::Format_Mono || img.format()==QImage::Format_MonoLSB,
               __func__,"Only monochrome image is valid.");
    const int whiteIndex = img.colorTable().first() == QColor(Qt::white).rgba() ? 0 : 1;

    QVector<QPoint> result;
    const int width = img.width();
    const int height = img.height();
    switch (img.format()) {
    case QImage::Format_Mono:
        for (int y=0; y<height; ++y)
        {
            const uchar* line = img.constScanLine(y);
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
            const uchar* line = img.constScanLine(y);
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
        break;
    }

    return result;
}

CircleData naiveCircleFit(const QVector<QPoint>& points)
{
    const int num = points.size();
    CircleData circle;

    QPoint sumP(0,0);
    for (const auto& point : points)
    {
        sumP += point;
    }
    circle.center = QPointF(sumP)/num;

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
    CircleData circle;
    const int num = points.size();
    if (num<3)
    {
        qWarning() << __func__ << ": Fitting a circle requires at least three points";
        return naiveCircleFit(points);
    }
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

    A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
    Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)

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

    QPoint sumP(0,0);
    for (const auto& point : points)
    {
        sumP += point;
    }
    QPointF mean = QPointF(sumP)/num;

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

    qreal a2 = 4*covxy - 3*mz*mz - mzz;
    qreal a1 = varz*mz + 4*covxy*mz - mxz*mxz - myz*myz;
    qreal a0 = mxz*(mxz*myy - myz*mxy) + myz*(myz*mxx - mxz*mxy) - varz*covxy;
    qreal a22 = 2*a2;

    qreal t=0, f=a0;
    constexpr uint maxIter = 99;
    for (uint iter=0; iter<maxIter; ++iter)
    {
        qreal df = a1 + t*(a22 + 16*t*t);
        qreal t_ = t - f/df;
        if (qFuzzyIsNull(t-t_) || !::std::isfinite(t_))
        {
            break;
        }
        qreal f_ = a0 + t_*(a1 + t_*(a2 + 4*t_*t_));
        if (abs(f_) >= abs(f))
        {
            break;
        }
        t = t_; f = f_;
    }
    qreal det = t*t - t*mz + covxy;
    qreal xCenter = (mxz*(myy - t) - myz*mxy) / det / 2;
    qreal yCenter = (myz*(mxx - t) - mxz*mxy) / det / 2;
    circle.center = mean + QPointF{xCenter, yCenter};
    circle.radius = ::std::sqrt(xCenter*xCenter + yCenter*yCenter + mz - 2*t);

    return circle;
}

} // namespace MEMS
