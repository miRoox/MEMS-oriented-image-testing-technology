#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include <QtGlobal>

class QImage;

namespace MEMS {

enum class AutoThresholdMethod
{
    Cluster     = 0,
    Otsu        = AutoThresholdMethod::Cluster,
    Mean        = 1,
    Moments     = 2,
    Fuzziness   = 3,
};

using Histogram = QVector<::std::size_t>;

extern int meanThreshold(const QImage& img);
extern int meanThreshold(const Histogram& histogram);
extern int pTileThreshold(const QImage& img, qreal pValue);
extern int pTileThreshold(const Histogram& histogram, qreal pValue);
extern int clusterThreshold(const QImage& img);
extern int clusterThreshold(const Histogram& histogram);
extern int momentsThreshold(const QImage& img);
extern int momentsThreshold(const Histogram& histogram);
extern int fuzzinessThreshold(const QImage& img);
extern int fuzzinessThreshold(const Histogram& histogram);

extern Histogram grayscaleHistogram(const QImage& image);

} // namespace MEMS

#endif // THRESHOLDING_H
