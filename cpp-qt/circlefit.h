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

extern QVector<QPoint> whitePixelPositions(const QImage& img);

extern CircleData naiveCircleFit(const QVector<QPoint>& points);
extern CircleData simpleAlgebraicCircleFit(const QVector<QPoint>& points);
extern CircleData hyperAlgebraicCircleFit(const QVector<QPoint>& points);

} // namespace MEMS

#endif // CIRCLEFIT_H
