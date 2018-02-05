#ifndef CIRCLEFIT_H
#define CIRCLEFIT_H

class QImage;
class QPoint;
template<typename T>
class QVector;

namespace MEMS {

extern QVector<QPoint> whitePixelPositions(const QImage& img);

} // namespace MEMS

#endif // CIRCLEFIT_H
