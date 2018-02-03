#ifndef EDGEDETECT_H
#define EDGEDETECT_H

class QImage;

namespace MEMS {

extern QImage sobelOperator(const QImage& img);
extern QImage prewittOperator(const QImage& img);
extern QImage scharrOperator(const QImage& img);
extern QImage laplacianOperator(const QImage& img);

} // namespace MEMS

#endif // EDGEDETECT_H
