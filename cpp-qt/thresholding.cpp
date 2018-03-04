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


#include "thresholding.h"

/*!
    \headerfile <thresholding.h>
    \title Image Thresholding Algorithms.
    \brief The <thresholding.h> header file provides various
    methods to automatically select the threshold.
 */

#include <QImage>
#include <QColor>
#include <QVector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <utils.h>

namespace MEMS {

static constexpr ::std::size_t ColorValueRange = 0x100;

/*!
    \enum AutoThresholdMethod

    This enum lists the supported methods for auto-thresholding.

    \value Cluster
           Clustering-based thresholding.
    \value Otsu
           Same as Cluster
    \value Mean
           Use the mean level as the threshold.
    \value Moments
           Moment-preserving thresholding.
    \value Fuzziness
           Thresholding by minimizing the measures of fuzziness.

    \omitvalue Cluster
    \omitvalue Otsu
    \omitvalue Mean
    \omitvalue Moments
    \omitvalue Fuzziness
 */


/*!
    Use the mean level as the threshold.
 */
int meanThreshold(const QImage& image)
{
    return meanThreshold(grayscaleHistogram(image));
}

/*!
    \overload meanThreshold
 */
int meanThreshold(const Histogram& histogram)
{
    int total = 0;
    int weighted = 0;
    const int histoSize = histogram.length();
    for (int i=0; i<histoSize; ++i)
    {
        total += histogram.at(i);
        weighted += i*histogram.at(i);

        PROGRESS_UPDATE(i/histoSize);
    }
    return weighted/total;
}

/*!
    Find a threshold value that make a fraction \a pValue
    of all pixels black.
 */
int pTileThreshold(const QImage& image, qreal pValue)
{
    return pTileThreshold(grayscaleHistogram(image),pValue);
}

/*!
    \overload pTileThreshold
 */
int pTileThreshold(const Histogram& histogram, qreal pValue)
{
    Q_ASSERT_X(pValue>=0&&pValue<1,__func__,"p-value is out of range.");

    const int histoSize = histogram.length();
    ulong total = ::std::accumulate(histogram.cbegin(),histogram.cend(),ulong(0));

    int threshold = histoSize;
    ulong partSum = 0;
    for (int i=0; i<histoSize; ++i)
    {
        partSum += histogram.at(i);
        if (partSum > total*pValue)
        {
            threshold = i;
            break;
        }

        PROGRESS_UPDATE(i/histoSize);
    }
    PROGRESS_UPDATE(0.99);

    return threshold;
}

/*!
    Perform clustering-based image thresholding, also known
    as Otsu's method.
 */
int clusterThreshold(const QImage& image)
{
/*!
    \quotation
    Otsu, N (1979), "A threshold selection method from gray-level histograms",
    IEEE Trans. Sys., Man., Cyber. 9: 62-66, doi:10.1109/TSMC.1979.4310076
    \endquotation
 */

    return clusterThreshold(grayscaleHistogram(image));
}

/*!
    \overload clusterThreshold
 */
int clusterThreshold(const Histogram& histogram)
{
    const int histoSize = histogram.length();
    qreal total = ::std::accumulate(histogram.cbegin(),histogram.cend(),qreal(0));
    QVector<qreal> normalizedHistogram(histoSize);

    qreal globalAverage = 0.;
    for (int i=0; i<histoSize; ++i)
    {
        normalizedHistogram[i] = histogram.at(i)/total;
        globalAverage += i*normalizedHistogram.at(i);
    }
    PROGRESS_UPDATE(0.5);

    int threshold = 0;
    qreal maxVariance = 0.;
    qreal average = 0.;
    qreal fraction = 0.;
    for (int i=0; i<histoSize ;++i)
    {
        fraction += normalizedHistogram.at(i);
        average += i*normalizedHistogram.at(i);
        qreal distance = average/fraction - globalAverage;
        qreal variance = distance*distance*fraction/(1-fraction);
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold =i;
        }
    }
    PROGRESS_UPDATE(0.99);

    return threshold;
}

/*!
    Moment-preserving thresholding.
 */
int momentsThreshold(const QImage& image)
{
/*!
    \quotation
    Tsai, W (1985), "Moment-preserving thresholding: a new approach",
    Computer Vision, Graphics, and Image Processing 29: 377-393
    \endquotation
 */

    return momentsThreshold(grayscaleHistogram(image));
}

/*!
    \overload momentsThreshold
 */
int momentsThreshold(const Histogram& histogram)
{
    using ::std::sqrt;

    qreal total = ::std::accumulate(histogram.cbegin(),histogram.cend(),qreal(0));
    const int histoSize = histogram.length();
    QVector<qreal> normalizedHistogram(histoSize);

    // calculate the first, second, and third order moments
    qreal m0=1.;
    qreal m1=0, m2=0, m3=0;
    for (int i=0; i<histoSize; ++i)
    {
        normalizedHistogram[i] = histogram.at(i)/total;
        m1 += i*normalizedHistogram.at(i);
        m2 += i*i*normalizedHistogram.at(i);
        m3 += i*i*i*normalizedHistogram.at(i);
    }

    qreal cd = m0*m2-m1*m1;
    qreal c0 = (m1*m3-m2*m2)/cd;
    qreal c1 = (m1*m2-m0*m3)/cd;
    qreal z0 = 0.5*(-sqrt(c1*c1-4*c0)-c1);
    qreal z1 = 0.5*( sqrt(c1*c1-4*c0)-c1);
    qreal pt = (z1-m1)/(z1-z0);

    return pTileThreshold(histogram,pt);
}

/*!
    Thresholding by minimizing the measures of fuzziness.
 */
int fuzzinessThreshold(const QImage& image)
{
/*!
    \quotation
    Huang, L-K & Wang, M-J J (1995), "Image thresholding by minimizing the measure of fuzziness",
    Pattern Recognition 28(1): 41-51
    \endquotation
 */

    return fuzzinessThreshold(grayscaleHistogram(image));
}

/*!
    \overload fuzzinessThreshold
 */
int fuzzinessThreshold(const Histogram& histogram)
{
    using ::std::log;
    using ::std::abs;

    const int histoSize = histogram.length();
    // find first and last non-empty color
    int first, last;
    for (first=0; first<histoSize && histogram[first]==0; ++first);
    for (last=histoSize-1; last>first && histogram[last]==0; --last);
    if (first==last || first+1==last)
        return first;
    PROGRESS_UPDATE(0.1);

    // calculate the cumulative density and the weighted cumulative density
    QVector<qreal> s(last+1,0);
    QVector<qreal> w(last+1,0);
    s[0] = histogram[0];
    for (int i= ::std::max(first,1); i<=last; ++i)
    {
        s[i] = s.at(i-1) + histogram.at(i);
        w[i] = w.at(i-1) + i*histogram.at(i);
    }
    PROGRESS_UPDATE(0.2);

    // precalculate the summands of the entropy given the absolute difference x-μ
    qreal c = last-first;
    QVector<qreal> s_mu(last-first+1);
    for (int i=1; i<s_mu.length(); ++i)
    {
        qreal mu = 1/(1+i/c);
        s_mu[i] = -mu*log(mu) - (1-mu)*log(1-mu);
    }
    PROGRESS_UPDATE(0.5);

    // find the threshold
    int threshold = 0;
    qreal bestEntropy = ::std::numeric_limits<qreal>::max();
    for (int i=first; i<last; ++i)
    {
        qreal entropy = 0.;
        int mu = static_cast<int>(w.at(i)/s.at(i));
        for (int j=first; j<=i; ++j)
        {
            entropy += s_mu.at(abs(j-mu))*histogram.at(j);
        }
        mu = static_cast<int>((w.at(last)-w.at(i))/(s.at(last)-s.at(i)));
        for (int j=i+1; j<=last; ++j)
        {
            entropy += s_mu.at(abs(j-mu))*histogram.at(j);
        }
        if (bestEntropy > entropy)
        {
            bestEntropy = entropy;
            threshold = i;
        }
    }
    PROGRESS_UPDATE(0.99);

    return threshold;
}

/*!
    Get grayscale histogram of the \a image .
 */
Histogram grayscaleHistogram(const QImage& image)
{
    Histogram histogram(ColorValueRange,0);

    int height = image.height();
    int width = image.width();
    for (int y=0 ;y<height ;++y)
    {
        for (int x=0 ;x<width ;++x)
        {
            histogram[qGray(image.pixel(x,y))] += 1;
        }
    }

    return histogram;
}

} // namespace MEMS
