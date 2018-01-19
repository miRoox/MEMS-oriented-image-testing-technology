#ifndef THRESHOLDING_HPP
#define THRESHOLDING_HPP

/*!
    \headerfile <thresholding.hpp>
    \title Image Thresholding Algorithms.
    \brief The <thresholding.hpp> header file provides various
    methods to automatically select the threshold.
 */

#include <QImage>
#include <QColor>
#include <QVector>
#include <cmath>
#include <limits>
#include <QtDebug>

namespace MEMS {

namespace ThresholdPrivate {

constexpr ::std::size_t ColorValueRange = 0x100;

}

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
enum class AutoThresholdMethod
{
    Cluster     = 0,
    Otsu        = AutoThresholdMethod::Cluster,
    Mean        = 1,
    Moments     = 2,
    Fuzziness   = 3,
};

inline QVector<::std::size_t> imageValueHistogram(const QImage& image);

/*!
    Use the mean level as the threshold.
 */
inline int meanThreshold(const QImage& img)
{
    qreal threshold = 0.;

    const int height = img.height();
    const int width = img.width();
    const int size = height*width;
    for (int y=0; y<height ;++y)
    {
        for (int x=0; x<width ;++x)
        {
            threshold += img.pixelColor(x,y).valueF()/size;
        }
    }
    return 0xff*threshold;
}

/*!
    \overload meanThreshold()
 */
inline int meanThreshold(const QVector<::std::size_t>& histogram)
{
    using namespace ThresholdPrivate;

    int total = 0;
    int weighted = 0;
    const int histoSize = histogram.length();
    for (int i=0; i<histoSize; ++i)
    {
        total += histogram.at(i);
        weighted += i*histogram.at(i);
    }
    return weighted/total;
}

/*!
    Find a threshold value that make a fraction \a pValue
    of all pixels black.
 */
inline int pTileThreshold(const QImage& img, qreal pValue)
{
    using namespace ThresholdPrivate;

    Q_ASSERT_X(pValue>=0&&pValue<1,__func__,"p-value is out of range.");

    const int height = img.height();
    const int width = img.width();
    const unsigned long total = height*width;
    const auto histogram = imageValueHistogram(img);

    int threshold = ColorValueRange;
    unsigned long partSum = 0;
    for (uint i=0; i<ColorValueRange; ++i)
    {
        partSum += histogram.at(i);
        if (partSum > total*pValue)
        {
            threshold = i;
            break;
        }
    }
    return threshold;
}

/*!
    \overload pTileThreshold()
 */
inline int pTileThreshold(const QVector<::std::size_t>& histogram, qreal pValue)
{
    Q_ASSERT_X(pValue>=0&&pValue<1,__func__,"p-value is out of range.");

    const int histoSize = histogram.length();
    unsigned long total = 0;
    for (const auto var : histogram)
    {
        total += var;
    }

    int threshold = histoSize;
    unsigned long partSum = 0;
    for (int i=0; i<histoSize; ++i)
    {
        partSum += histogram.at(i);
        if (partSum > total*pValue)
        {
            threshold = i;
            break;
        }
    }
    return threshold;
}

// pre-declaration
inline int clusterThreshold(const QVector<::std::size_t>& histogram);

/*!
    Perform clustering-based image thresholding, also known
    as Otsu's method.
 */
inline int clusterThreshold(const QImage& img)
{
/*!
    \quotation
    Otsu, N (1979), "A threshold selection method from gray-level histograms",
    IEEE Trans. Sys., Man., Cyber. 9: 62-66, doi:10.1109/TSMC.1979.4310076
    \endquotation
 */

    return clusterThreshold(imageValueHistogram(img));
}

/*!
    \overload clusterThreshold()
 */
inline int clusterThreshold(const QVector<::std::size_t>& histogram)
{
    const int histoSize = histogram.length();
    qreal total = 0.;
    QVector<qreal> normalizedHistogram(histoSize);
    for (const auto var : histogram)
    {
        total += var;
    }

    qreal globalAverage = 0.;
    for (int i=0; i<histoSize; ++i)
    {
        normalizedHistogram[i] = histogram.at(i)/total;
        globalAverage += i*normalizedHistogram.at(i);
    }

    int threshold = 0;
    qreal maxVariance = 0.;
    qreal average = 0.;
    qreal foregroundProp = 0.;
    for (int i=0; i<histoSize ;++i)
    {
        foregroundProp += normalizedHistogram.at(i);
        average += i*normalizedHistogram.at(i);
        qreal tmp = average/foregroundProp - globalAverage;
        qreal variance = tmp*tmp*foregroundProp/(1-foregroundProp);
        if (variance>maxVariance)
        {
            maxVariance = variance;
            threshold =i;
        }
    }

    return threshold;
}

// pre-declaration
inline int momentsThreshold(const QVector<::std::size_t>& histogram);

/*!
    Moment-preserving thresholding.
 */
inline int momentsThreshold(const QImage& img)
{
/*!
    \quotation
    Tsai, W (1985), "Moment-preserving thresholding: a new approach",
    Computer Vision, Graphics, and Image Processing 29: 377-393
    \endquotation
 */

    return momentsThreshold(imageValueHistogram(img));
}

/*!
    \overload momentsThreshold()
 */
inline int momentsThreshold(const QVector<::std::size_t>& histogram)
{
    using ::std::sqrt;

    qreal total = 0.;
    const int histoSize = histogram.length();
    QVector<qreal> normalizedHistogram(histoSize);
    for (const auto var : histogram)
    {
        total += var;
    }

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

    qreal partSum = 0.;
    int threshold = histoSize;
    for (int i=0; i<histoSize; ++i)
    {
        partSum += normalizedHistogram.at(i);
        if (partSum > pt)
        {
            threshold = i;
            break;
        }
    }
    return threshold;
}

// pre-declaration
inline int fuzzinessThreshold(const QVector<::std::size_t>& histogram);

/*!
    Thresholding by minimizing the measures of fuzziness.
 */
inline int fuzzinessThreshold(const QImage& img)
{
/*!
    \quotation
    Huang, L-K & Wang, M-J J (1995), "Image thresholding by minimizing the measure of fuzziness",
    Pattern Recognition 28(1): 41-51
    \endquotation
 */

    return fuzzinessThreshold(imageValueHistogram(img));
}

/*!
    \overload fuzzinessThreshold()
 */
inline int fuzzinessThreshold(const QVector<::std::size_t>& histogram)
{
    using ::std::log;
    using ::std::abs;

    const int histoSize = histogram.length();
    int first, last;

    for (first=0; first<histoSize && histogram[first]==0; ++first);
    for (last=histoSize-1; last>first && histogram[last]==0; --last);
    if (first==last || first+1==last)
        return first;

    QVector<qreal> s(last+1,0);
    QVector<qreal> w(last+1,0);
    s[0] = histogram[0];
    for (int i= ::std::max(first,1); i<=last; ++i)
    {
        s[i] = s.at(i-1) + histogram.at(i);
        w[i] = w.at(i-1) + i*histogram.at(i);
    }

    qreal c = last-first;
    QVector<qreal> s_mu(last-first+1);
    for (int i=1; i<s_mu.length(); ++i)
    {
        qreal mu = 1/(1+i/c);
        s_mu[i] = -mu*log(mu) - (1-mu)*log(1-mu);
    }

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
    return threshold;
}

/*!
    Get HSV-value histogram of the \a image .
 */
inline QVector<::std::size_t> imageValueHistogram(const QImage& image)
{
    using namespace ThresholdPrivate;

    QVector<::std::size_t> histogram(ColorValueRange,0);

    int height = image.height();
    int width = image.width();
    for (int y=0 ;y<height ;++y)
    {
        for (int x=0 ;x<width ;++x)
        {
            histogram[image.pixelColor(x,y).value()] += 1;
        }
    }

    return histogram;
}

} // namespace MEMS

#endif // THRESHOLDING_HPP
