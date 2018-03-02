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
        uchar* line = binarized.scanLine(y);
        for (int x=0; x<width ;++x)
        {
            if (predicate(origin.pixel(x,y)))
            {
                line[x>>3] |= (1 << (7-(x & 7)));
            }
            else
            {
                line[x>>3] &= ~(1 << (7-(x & 7)));
            }
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
    Q_ASSUME(threshold>=0&&threshold<0x100);
    return binarize(origin,[=](QRgb pixel){return qGray(pixel)>threshold;});
}

/*!
    \overload binarize

    Create a binary image from \a origin. Use a certain \a method to
    find the threshold.

    Default \a method is Otsu's method.

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
        Q_UNREACHABLE();
        break;
    }
    return binarize(origin);
}

} // namespace MEMS

#endif // BINARIZE_HPP
