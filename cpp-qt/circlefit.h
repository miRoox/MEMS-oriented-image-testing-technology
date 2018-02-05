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
    CircleData() : center(0,0),radius(0) {}

    QPointF center;
    qreal radius;
};

extern QVector<QPoint> whitePixelPositions(const QImage& img);

extern CircleData naiveCircleFit(const QVector<QPoint>& points);

} // namespace MEMS

#endif // CIRCLEFIT_H
