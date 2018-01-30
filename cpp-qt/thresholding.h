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

extern int meanThreshold(const QImage& img);
extern int meanThreshold(const QVector<::std::size_t>& histogram);
extern int pTileThreshold(const QImage& img, qreal pValue);
extern int pTileThreshold(const QVector<::std::size_t>& histogram, qreal pValue);
extern int clusterThreshold(const QImage& img);
extern int clusterThreshold(const QVector<::std::size_t>& histogram);
extern int momentsThreshold(const QImage& img);
extern int momentsThreshold(const QVector<::std::size_t>& histogram);
extern int fuzzinessThreshold(const QImage& img);
extern int fuzzinessThreshold(const QVector<::std::size_t>& histogram);

extern QVector<::std::size_t> grayscaleHistogram(const QImage& image);

} // namespace MEMS

#endif // THRESHOLDING_H
