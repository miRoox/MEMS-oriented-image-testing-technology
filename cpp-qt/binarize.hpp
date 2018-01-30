#ifndef BINARIZE_HPP
#define BINARIZE_HPP

/*!
    \headerfile <binarize.hpp>
    \title Image Binarize Algorithms
    \brief The <binarize.hpp> header file provides image binarize
    algorithms.

    \sa <thresholding.h>
 */

#include <QImage>
#include <QColor>

#include "thresholding.h"

namespace MEMS {

/*!
    \fn QImage binarize(const QImage& origin, Predicate predicate)

    Create a binary image from \a origin, by replacing pixels which
    \c predicate(pixel) is true with 1 and others with 0.
 */
template<typename Predicate>
QImage binarize(const QImage& origin, Predicate predicate)
{
    QImage binarized(origin.size(),QImage::Format_Mono);

    int height = origin.height();
    int width = origin.width();
    for (int y=0; y<height ;++y)
    {
        for (int x=0; x<width ;++x)
        {
            binarized.setPixel(x,y,
                               predicate(origin.pixel(x,y))
                               ? Qt::color1 : Qt::color0);
        }
    }

    return binarized;
}

/*!
    \overload binarize

    Create a binary image from \a origin, by replacing pixels above the
    global \a threshold with 1 and others with 0.
 */
inline QImage binarize(const QImage& origin, int threshold)
{
    Q_ASSERT_X(threshold>=0&&threshold<0x100,
               __func__,"threshold is out of range");
    return binarize(origin,[=](QRgb pixel){return qGray(pixel)>threshold;});
}

/*!
    \overload binarize

    Create a binary image from \a origin. Use a certain \a method to
    find the threshold.

    Defaulf \a method is Otsu's method.

    \sa AutoThresholdMethod
 */
inline QImage binarize(const QImage& origin,
                       AutoThresholdMethod method = AutoThresholdMethod::Otsu)
{
    switch (method)
    {
    case AutoThresholdMethod::Cluster:
        return binarize(origin,clusterThreshold(origin));
    case AutoThresholdMethod::Mean:
        return binarize(origin,meanThreshold(origin));
    case AutoThresholdMethod::Moments:
        return binarize(origin,momentsThreshold(origin));
    case AutoThresholdMethod::Fuzziness:
        return binarize(origin,fuzzinessThreshold(origin));
    default:
        break;
    }
    return binarize(origin);
}

} // namespace MEMS

#endif // BINARIZE_HPP
