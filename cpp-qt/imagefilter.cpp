#include "imagefilter.h"
#include <QImage>
#include <QPoint>
#include <QColor>
#include <cmath>

#include <QtDebug>

namespace MEMS {

/*!
    \internal
 */
static inline int calcPaddingX(const int width, int x, PaddingType padding)
{
    switch (padding)
    {
    case PaddingType::Fixed:
        if (x<0)
            x = 0;
        else if (x>=width)
            x = width-1;
        break;
    case PaddingType::Periodic:
        x = x>=0 ? x%width
                 : x%width+width;
        break;
    case PaddingType::Reflected:
        while (x>=0 && x<width)
        {
            if (x<0)
                x = -x-1;
            else
                x = 2*width-x-1;
        }
    default:
        break;
    }
    return x;
}

/*!
    \internal
 */
static inline int calcPaddingY(const int height, int y, PaddingType padding)
{
    switch (padding)
    {
    case PaddingType::Fixed:
        if (y<0)
            y = 0;
        else if (y>=height)
            y = height-1;
        break;
    case PaddingType::Periodic:
        y = y>=0 ? y%height
                 : y%height+height;
        break;
    case PaddingType::Reflected:
        while (y>=0 && y<height)
        {
            if (y<0)
                y = -y-1;
            else
                y = 2*height-y-1;
        }
        break;
    default:
        break;
    }
    return y;
}

QImage convolve(const QImage& origin, const MatrixKernel& kernel, PaddingType padding)
{
    QImage output(origin.size(),QImage::Format_RGB32);
    const QImage input = origin.convertToFormat(QImage::Format_RGB32);

    const int width = origin.width();
    const int height = origin.height();
    const int kerRows = kernel.size();
    const int kerCols = kernel.first().size();
    Q_ASSERT_X(kerRows%2==1,__func__,"Row of kernel must be odd");
    Q_ASSERT_X(kerCols%2==1,__func__,"Column of kernel must be odd");
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;
    for (const auto& row : kernel)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel is not a matrix");
    }

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            qreal rr=0, gg=0, bb=0;
            for (int i=0; i<kerRows; ++i)
            {
                const int yy = calcPaddingY(height,y+i-kerCenterY,padding);
                const QRgb* iLine = reinterpret_cast<const QRgb*>(input.constScanLine(yy));
                for (int j=0; j<kerCols; ++j)
                {
                    const int xx = calcPaddingX(width,x+j-kerCenterX,padding);
                    const auto kerVar = kernel.at(kerRows-1-i).at(kerCols-1-j);
                    rr += qRed(iLine[xx])*kerVar;
                    gg += qGreen(iLine[xx])*kerVar;
                    bb += qBlue(iLine[xx])*kerVar;
                }
            }
            line[x] = qRgb(qBound(0,static_cast<int>(rr),0xff),
                           qBound(0,static_cast<int>(gg),0xff),
                           qBound(0,static_cast<int>(bb),0xff));
        }
    }

    return output.convertToFormat(origin.format());
}

QImage convolve(const QImage& origin, const MatrixKernel& kernel, QRgb padding)
{
    QImage output(origin.size(),QImage::Format_RGB32);

    const int width = origin.width();
    const int height = origin.height();
    const int kerRows = kernel.size();
    const int kerCols = kernel.first().size();
    Q_ASSERT_X(kerRows%2==1,__func__,"Row of kernel must be odd");
    Q_ASSERT_X(kerCols%2==1,__func__,"Column of kernel must be odd");
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;
    for (const auto& row : kernel)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel is not a matrix");
    }

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            qreal rr=0, gg=0, bb=0;
            for (int i=0; i<kerRows; ++i)
            {
                for (int j=0; j<kerCols; ++j)
                {
                    const QRgb pix = origin.valid(x+j-kerCenterX,y+i-kerCenterY)
                                    ? origin.pixel(x+j-kerCenterX,y+i-kerCenterY)
                                    : padding;
                    const auto kerVar = kernel.at(kerRows-1-i).at(kerCols-1-j);
                    rr += qRed(pix)*kerVar;
                    gg += qGreen(pix)*kerVar;
                    bb += qBlue(pix)*kerVar;
                }
            }
            line[x] = qRgb(qBound(0,static_cast<int>(rr),0xff),
                           qBound(0,static_cast<int>(gg),0xff),
                           qBound(0,static_cast<int>(bb),0xff));
        }
    }

    return output.convertToFormat(origin.format());
}

QImage convolve(const QImage& origin, const MatrixKernel& kernel, const QColor& padding)
{
    return convolve(origin,kernel,padding.rgb());
}

QImage convolveXY(const QImage& origin, const MatrixKernel& kerX, const MatrixKernel& kerY, PaddingType padding)
{
    using ::std::hypot;

    QImage output(origin.size(),QImage::Format_RGB32);
    const QImage input = origin.convertToFormat(QImage::Format_RGB32);

    const int width = origin.width();
    const int height = origin.height();
    Q_ASSERT_X(kerX.size()==kerY.size(),__func__,"Size of Kernel X and Kernel Y must be same");
    Q_ASSERT_X(kerX.first().size()==kerY.first().size(),__func__,"Size of Kernel X and Kernel Y must be same");
    const int kerRows = kerX.size();
    const int kerCols = kerX.first().size();
    Q_ASSERT_X(kerRows%2==1,__func__,"Row of kernels must be odd");
    Q_ASSERT_X(kerCols%2==1,__func__,"Column of kernels must be odd");
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;
    for (const auto& row : kerX)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel X is not a matrix");
    }
    for (const auto& row : kerY)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel Y is not a matrix");
    }

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            qreal rx=0, gx=0, bx=0;
            qreal ry=0, gy=0, by=0;
            for (int i=0; i<kerRows; ++i)
            {
                const int yy = calcPaddingY(height,y+i-kerCenterY,padding);
                const QRgb* iLine = reinterpret_cast<const QRgb*>(input.constScanLine(yy));
                for (int j=0; j<kerCols; ++j)
                {
                    const int xx = calcPaddingX(width,x+j-kerCenterX,padding);
                    const auto kerXVar = kerX.at(kerRows-1-i).at(kerCols-1-j);
                    const auto kerYVar = kerY.at(kerRows-1-i).at(kerCols-1-j);
                    rx += qRed(iLine[xx])*kerXVar;
                    gx += qGreen(iLine[xx])*kerXVar;
                    bx += qBlue(iLine[xx])*kerXVar;
                    ry += qRed(iLine[xx])*kerYVar;
                    gy += qGreen(iLine[xx])*kerYVar;
                    by += qBlue(iLine[xx])*kerYVar;
                }
            }
            line[x] = qRgb(qBound(0,static_cast<int>(hypot(rx,ry)),0xff),
                           qBound(0,static_cast<int>(hypot(gx,gy)),0xff),
                           qBound(0,static_cast<int>(hypot(bx,by)),0xff));
        }
    }

    return output.convertToFormat(origin.format());
}

QImage convolveXY(const QImage& origin, const MatrixKernel& kerX, const MatrixKernel& kerY, QRgb padding)
{
    using ::std::hypot;

    QImage output(origin.size(),QImage::Format_RGB32);

    const int width = origin.width();
    const int height = origin.height();
    Q_ASSERT_X(kerX.size()==kerY.size(),__func__,"Size of Kernel X and Kernel Y must be same");
    Q_ASSERT_X(kerX.first().size()==kerY.first().size(),__func__,"Size of Kernel X and Kernel Y must be same");
    const int kerRows = kerX.size();
    const int kerCols = kerX.first().size();
    Q_ASSERT_X(kerRows%2==1,__func__,"Row of kernels must be odd");
    Q_ASSERT_X(kerCols%2==1,__func__,"Column of kernels must be odd");
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;
    for (const auto& row : kerX)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel X is not a matrix");
    }
    for (const auto& row : kerY)
    {
        Q_ASSERT_X(row.size()==kerCols,__func__,"Kernel Y is not a matrix");
    }

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            qreal rx=0, gx=0, bx=0;
            qreal ry=0, gy=0, by=0;
            for (int i=0; i<kerRows; ++i)
            {
                for (int j=0; j<kerCols; ++j)
                {
                    const QRgb pix = origin.valid(x+j-kerCenterX,y+i-kerCenterY)
                                    ? origin.pixel(x+j-kerCenterX,y+i-kerCenterY)
                                    : padding;
                    const auto kerXVar = kerX.at(kerRows-1-i).at(kerCols-1-j);
                    const auto kerYVar = kerY.at(kerRows-1-i).at(kerCols-1-j);
                    rx += qRed(pix)*kerXVar;
                    gx += qGreen(pix)*kerXVar;
                    bx += qBlue(pix)*kerXVar;
                    ry += qRed(pix)*kerYVar;
                    gy += qGreen(pix)*kerYVar;
                    by += qBlue(pix)*kerYVar;
                }
            }
            line[x] = qRgb(qBound(0,static_cast<int>(hypot(rx,ry)),0xff),
                           qBound(0,static_cast<int>(hypot(gx,gy)),0xff),
                           qBound(0,static_cast<int>(hypot(bx,by)),0xff));
        }
    }

    return output.convertToFormat(origin.format());
}

QImage convolveXY(const QImage& origin, const MatrixKernel& kerX, const MatrixKernel& kerY, const QColor& padding)
{
    return convolveXY(origin,kerX,kerY,padding.rgb());
}

QImage boxFilter(const QImage& origin, uint radius, PaddingType padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

QImage boxFilter(const QImage& origin, uint radius, QRgb padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

QImage boxFilter(const QImage& origin, uint radius, const QColor& padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

/*!
    \internal
 */
static inline MatrixKernel gaussianKernel(uint radius, qreal sigma)
{
    constexpr qreal PI = 3.14159265358979323846;

    MatrixKernel ker(2*radius+1,QVector<qreal>(2*radius+1,0));
    const int r = radius;
    const qreal s = 2*sigma*sigma;
    qreal sum = 0;

    for (int x=-r; x<=r; ++x)
    {
        for (int y=-r; y<=r; ++y)
        {
            sum += ker[x+radius][y+radius] = ::std::exp(-(x*x+y*y)/s)/s/PI;
        }
    }
    for (auto& row : ker)
    {
        for (auto& val : row)
        {
            val /= sum;
        }
    }

    return ker;
}

QImage gaussianFilter(const QImage& origin, uint radius, PaddingType padding)
{
    return gaussianFilter(origin,radius,radius/2.,padding);
}

QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma, PaddingType padding)
{
    return convolve(origin,gaussianKernel(radius,sigma),padding);
}

QImage gaussianFilter(const QImage& origin, uint radius, QRgb padding)
{
    return gaussianFilter(origin,radius,radius/2.,padding);
}

QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma, QRgb padding)
{
    return convolve(origin,gaussianKernel(radius,sigma),padding);
}

QImage gaussianFilter(const QImage& origin, uint radius, const QColor& padding)
{
    return gaussianFilter(origin,radius,radius/2.,padding);
}

QImage gaussianFilter(const QImage& origin, uint radius, qreal sigma, const QColor& padding)
{
    return convolve(origin,gaussianKernel(radius,sigma),padding);
}

/*!
    \internal

    when the orgin image is grayscale.
 */
static QImage medianFilterA(const QImage& origin, uint radius)
{
    Q_ASSERT_X(origin.isGrayscale(),__func__,
               "Only grayscale image can use this algorithm.");

    QImage output(origin.size(),QImage::Format_RGB32);
    const QImage input = origin.convertToFormat(QImage::Format_RGB32);
    QVector<uint> histogram(0x100,0);

    const int width = origin.width();
    const int height = origin.height();
    const int r = radius;

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            uint sum = 0;
            histogram.fill(0);
            for (int i=-r; i<=r; ++i)
            {
                const int yy = y+i;
                if (yy<0 || yy>=height)
                    continue;
                const QRgb* iLine = reinterpret_cast<const QRgb*>(input.constScanLine(yy));
                for (int j=-r; j<=r; ++j)
                {
                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    ++histogram[qGray(iLine[xx])];
                    ++sum;
                }
            }

            uint partSum = 0;
            for (uint i=0; i<0x100;++i)
            {
                partSum += histogram.at(i);
                if (partSum>=sum/2)
                {
                    line[x] = qRgb(i,i,i);
                    break;
                }
            }
        }
    }

    return output.convertToFormat(origin.format());
}

/*!
    \internal

    when the radius is small, and the orgin image is not grayscale.
 */
static QImage medianFilterB(const QImage& origin, uint radius)
{
    QImage output(origin.size(),QImage::Format_RGB32);
    const QImage input = origin.convertToFormat(QImage::Format_RGB32);
    QVector<QRgb> medianCandidate;
    medianCandidate.reserve(((2*radius+1)*(2*radius+1))); // almost

    const int width = origin.width();
    const int height = origin.height();
    const int r = radius;

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            for (int i=-r; i<=r; ++i)
            {
                const int yy = y+i;
                if (yy<0 || yy>=height)
                    continue;
                const QRgb* iLine = reinterpret_cast<const QRgb*>(input.constScanLine(yy));
                for (int j=-r; j<=r; ++j)
                {
                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    medianCandidate.append(iLine[xx]);
                }
            }
            ::std::size_t medianIndex = medianCandidate.length()/2;
            ::std::nth_element(medianCandidate.begin(),
                               medianCandidate.begin()+medianIndex,
                               medianCandidate.end(),
                               [](QRgb a, QRgb b){
                return qGray(a) < qGray(b);
            });
            line[x] = medianCandidate.at(medianIndex);
            medianCandidate.clear();
        }
    }

    return output.convertToFormat(origin.format());
}

/*!
    \internal

    when the radius is large, and the orgin image is not grayscale.
 */
static QImage medianFilterC(const QImage& origin, uint radius)
{
    QImage output(origin.size(),QImage::Format_RGB32);
    const QImage input = origin.convertToFormat(QImage::Format_RGB32);
    QVector<QVector<QRgb>> histogram(0x100);
    for (auto& pixels : histogram)
    {
        pixels.reserve(2*radius*radius); // approximation
    }

    const int width = origin.width();
    const int height = origin.height();
    const int r = radius;

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        for (int x=0; x<width; ++x)
        {
            uint sum = 0;
            for (int i=-r; i<=r; ++i)
            {
                const int yy = y+i;
                if (yy<0 || yy>=height)
                    continue;
                const QRgb* iLine = reinterpret_cast<const QRgb*>(input.constScanLine(yy));
                for (int j=-r; j<=r; ++j)
                {
                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    histogram[qGray(iLine[xx])].append(iLine[xx]);
                    ++sum;
                }
            }

            bool find = false;
            uint partSum = 0;
            for (uint i=0; i<0x100;++i)
            {
                const uint nextSum = partSum + histogram.at(i).length();
                if (nextSum>sum/2 && !find)
                {
                    line[x] = histogram.at(i).at(sum/2-partSum);
                    find = true;
                }
                partSum = nextSum;
                histogram[i].clear();
            }
        }
    }

    return output.convertToFormat(origin.format());
}

QImage medianFilter(const QImage& origin, uint radius)
{
    return origin.isGrayscale()
            ? medianFilterA(origin,radius)
            : radius<10
              ? medianFilterB(origin,radius)
              : medianFilterC(origin,radius);
}

} // namespace MEMS
