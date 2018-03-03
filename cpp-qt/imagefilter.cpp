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


#include "imagefilter.h"

/*!
    \headerfile <imagefilter.h>
    \title Image Filter Algorithms
    \brief The <imagefilter.h> header file provide some normal image filter
 */

#include <QImage>
#include <QPoint>
#include <QColor>
#include "global.h"

namespace MEMS {

/*!
    \class MatrixKernel

    \brief Type of kernel for convolution.
 */

/*!
    \enum PaddingType

    This enum lists the padding types for convolution & convolution-based filters,
    which specifies what padding to use when extending beyond the original data specified.

    \value Fixed
           Repetitions of the elements on each boundary.
    \value Periodic
           Cyclic repetitions of the complete image.
    \value Reflected
           Reflections of the image in the boundary.

    \omitvalue Fixed
    \omitvalue Periodic
    \omitvalue Reflected
 */

MatrixKernel::MatrixKernel(const MatrixKernel& other)
    : data(other.data)
{
}

MatrixKernel::MatrixKernel(int rows, int columns, qreal value)
    : data(rows, QVector<qreal>(columns,value))
{
}

MatrixKernel::MatrixKernel(const QVector<QVector<qreal>>& args)
    : data(args)
{
    const int cols = columns();
    for (const auto& row : data)
    {
        Q_ASSERT_X(row.size()==cols,__func__,"Kernel is not a matrix");
    }
}

MatrixKernel::MatrixKernel(QVector<QVector<qreal>>&& args)
    : data(args)
{
    const int cols = columns();
    for (const auto& row : data)
    {
        Q_ASSERT_X(row.size()==cols,__func__,"Kernel is not a matrix");
    }
}

int MatrixKernel::rows() const
{
    return data.size();
}

int MatrixKernel::columns() const
{
    return data.first().size();
}

qreal MatrixKernel::at(int row, int column) const
{
    return data.at(row).at(column);
}

qreal &MatrixKernel::operator()(int row, int column)
{
    return data[row][column];
}

qreal MatrixKernel::operator()(int row, int column) const
{
    return data[row][column];
}

MatrixKernel& MatrixKernel::operator =(const MatrixKernel& other)
{
    data = other.data;
    return *this;
}

MatrixKernel& MatrixKernel::operator *=(qreal scaler)
{
    for (auto& row : data)
    {
        for (auto& val : row)
        {
            val *= scaler;
        }
    }
    return *this;
}

MatrixKernel& MatrixKernel::operator /=(qreal scaler)
{
    for (auto& row : data)
    {
        for (auto& val : row)
        {
            val /= scaler;
        }
    }
    return *this;
}

MatrixKernel MatrixKernel::operator *(qreal scaler) const
{
    MatrixKernel mat(*this);
    mat *= scaler;
    return mat;
}

MatrixKernel MatrixKernel::operator /(qreal scaler) const
{
    MatrixKernel mat(*this);
    mat /= scaler;
    return mat;
}

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
        Q_UNREACHABLE();
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
        Q_UNREACHABLE();
        break;
    }
    return y;
}

/*!
    Convolve the \a image with \a kernel, using specified padding type \a padding
 */
QImage convolve(const QImage& image, const MatrixKernel& kernel, PaddingType padding)
{
    QImage output(image.size(),QImage::Format_RGB32);
    const QImage input = image.convertToFormat(QImage::Format_RGB32);

    const int width = image.width();
    const int height = image.height();
    const int kerRows = kernel.rows();
    const int kerCols = kernel.columns();
    Q_ASSUME(kerRows%2==1);
    Q_ASSUME(kerCols%2==1);
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;

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
                    MAYBE_INTERRUPT();

                    const int xx = calcPaddingX(width,x+j-kerCenterX,padding);
                    const auto kerVar = kernel.at(kerRows-1-i,kerCols-1-j);
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

    return output.convertToFormat(image.format());
}

/*!
    \overload convolve

    Convolve the \a image with \a kernel, using specified padding color \a padding
 */
QImage convolve(const QImage& image, const MatrixKernel& kernel, QRgb padding)
{
    QImage output(image.size(),QImage::Format_RGB32);

    const int width = image.width();
    const int height = image.height();
    const int kerRows = kernel.rows();
    const int kerCols = kernel.columns();
    Q_ASSUME(kerRows%2==1);
    Q_ASSUME(kerCols%2==1);
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;

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
                    MAYBE_INTERRUPT();

                    const QRgb pix = image.valid(x+j-kerCenterX,y+i-kerCenterY)
                                    ? image.pixel(x+j-kerCenterX,y+i-kerCenterY)
                                    : padding;
                    const auto kerVar = kernel.at(kerRows-1-i,kerCols-1-j);
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

    return output.convertToFormat(image.format());
}

/*!
    \overload convolve

    Convolve the \a image with \a kernel, using specified padding color \a padding
 */
QImage convolve(const QImage& image, const MatrixKernel& kernel, const QColor& padding)
{
    return convolve(image,kernel,padding.rgb());
}

/*!
    Convolve the \a image with kernel \a kerX & \b kerY, using specified padding type \a padding,
    and then combine the two.
 */
QImage convolveXY(const QImage& image, const MatrixKernel& kerX, const MatrixKernel& kerY, PaddingType padding)
{
    using ::std::hypot;

    QImage output(image.size(),QImage::Format_RGB32);
    const QImage input = image.convertToFormat(QImage::Format_RGB32);

    const int width = image.width();
    const int height = image.height();
    Q_ASSUME(kerX.rows()==kerY.rows());
    Q_ASSUME(kerX.columns()==kerY.columns());
    const int kerRows = kerX.rows();
    const int kerCols = kerX.columns();
    Q_ASSUME(kerRows%2==1);
    Q_ASSUME(kerCols%2==1);
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;

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
                    MAYBE_INTERRUPT();

                    const int xx = calcPaddingX(width,x+j-kerCenterX,padding);
                    const auto kerXVar = kerX.at(kerRows-1-i,kerCols-1-j);
                    const auto kerYVar = kerY.at(kerRows-1-i,kerCols-1-j);
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

    return output.convertToFormat(image.format());
}

/*!
    \overload convolveXY

    Convolve the \a image with kernel \a kerX & \b kerY, using specified padding color \a padding,
    and then combine the two.
 */
QImage convolveXY(const QImage& image, const MatrixKernel& kerX, const MatrixKernel& kerY, QRgb padding)
{
    using ::std::hypot;

    QImage output(image.size(),QImage::Format_RGB32);

    const int width = image.width();
    const int height = image.height();
    Q_ASSUME(kerX.rows()==kerY.rows());
    Q_ASSUME(kerX.columns()==kerY.columns());
    const int kerRows = kerX.rows();
    const int kerCols = kerX.columns();
    Q_ASSUME(kerRows%2==1);
    Q_ASSUME(kerCols%2==1);
    const int kerCenterX = kerCols/2;
    const int kerCenterY = kerRows/2;

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
                    MAYBE_INTERRUPT();

                    const QRgb pix = image.valid(x+j-kerCenterX,y+i-kerCenterY)
                                    ? image.pixel(x+j-kerCenterX,y+i-kerCenterY)
                                    : padding;
                    const auto kerXVar = kerX.at(kerRows-1-i,kerCols-1-j);
                    const auto kerYVar = kerY.at(kerRows-1-i,kerCols-1-j);
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

    return output.convertToFormat(image.format());
}

/*!
    \overload convolveXY

    Convolve the \a image with kernel \a kerX & \b kerY, using specified padding color \a padding,
    and then combine the two.
 */
QImage convolveXY(const QImage& image, const MatrixKernel& kerX, const MatrixKernel& kerY, const QColor& padding)
{
    return convolveXY(image,kerX,kerY,padding.rgb());
}

/*!
    Filter \a image by convolving with a box kernel of \a radius.

    Every element of a box kernel is same, and the sum of the elements is 1.
 */
QImage boxFilter(const QImage& image, uint radius, PaddingType padding)
{
    MatrixKernel boxKer(2*radius+1,2*radius+1,
                        1./(2*radius+1)/(2*radius+1));
    return convolve(image,boxKer,padding);
}

/*!
    \overload boxFilter
 */
QImage boxFilter(const QImage& image, uint radius, QRgb padding)
{
    MatrixKernel boxKer(2*radius+1,2*radius+1,
                        1./(2*radius+1)/(2*radius+1));
    return convolve(image,boxKer,padding);
}

/*!
    \overload boxFilter
 */
QImage boxFilter(const QImage& image, uint radius, const QColor& padding)
{
    MatrixKernel boxKer(2*radius+1,2*radius+1,
                        1./(2*radius+1)/(2*radius+1));
    return convolve(image,boxKer,padding);
}

/*!
    \internal

    Generate Gaussian kernel with specified \a radius and \a sigma.
 */
static inline MatrixKernel gaussianKernel(uint radius, qreal sigma)
{
    constexpr qreal PI = 3.14159265358979323846;

    MatrixKernel ker(2*radius+1,2*radius+1,0);
    const int r = radius;
    const qreal s = 2*sigma*sigma;
    qreal sum = 0;

    for (int x=-r; x<=r; ++x)
    {
        for (int y=-r; y<=r; ++y)
        {
            sum += ker(x+r,y+r) = ::std::exp(-(x*x+y*y)/s)/s/PI;
        }
    }

    return ker/sum;
}

/*!
    Filter \a image by convolving with a Gaussian kernel of \a radius.

    Uses the standard deviation σ=radius/2.
 */
QImage gaussianFilter(const QImage& image, uint radius, PaddingType padding)
{
    return gaussianFilter(image,radius,radius/2.,padding);
}

/*!
    \overload gaussianFilter

    Uses a Gaussian kernel with \a radius and standard deviation \a sigma.
 */
QImage gaussianFilter(const QImage& image, uint radius, qreal sigma, PaddingType padding)
{
    return convolve(image,gaussianKernel(radius,sigma),padding);
}

/*!
    \overload gaussianFilter
 */
QImage gaussianFilter(const QImage& image, uint radius, QRgb padding)
{
    return gaussianFilter(image,radius,radius/2.,padding);
}

/*!
    \overload gaussianFilter
 */
QImage gaussianFilter(const QImage& image, uint radius, qreal sigma, QRgb padding)
{
    return convolve(image,gaussianKernel(radius,sigma),padding);
}

/*!
    \overload gaussianFilter
 */
QImage gaussianFilter(const QImage& image, uint radius, const QColor& padding)
{
    return gaussianFilter(image,radius,radius/2.,padding);
}

/*!
    \overload gaussianFilter
 */
QImage gaussianFilter(const QImage& image, uint radius, qreal sigma, const QColor& padding)
{
    return convolve(image,gaussianKernel(radius,sigma),padding);
}

/*!
    \internal

    when the orgin image is grayscale.
 */
static QImage medianFilter_Grayscale(const QImage& image, uint radius)
{
    Q_ASSUME(image.isGrayscale());

    QImage output(image.size(),QImage::Format_Grayscale8);
    const QImage input = image.convertToFormat(QImage::Format_Grayscale8);
    QVector<uint> histogram(0x100,0);

    const int width = image.width();
    const int height = image.height();
    const int r = radius;

    for (int y=0; y<height; ++y)
    {
        uchar* line = output.scanLine(y);
        for (int x=0; x<width; ++x)
        {
            uint sum = 0;
            histogram.fill(0);
            for (int i=-r; i<=r; ++i)
            {
                const int yy = y+i;
                if (yy<0 || yy>=height)
                    continue;
                const uchar* iLine = input.constScanLine(yy);
                for (int j=-r; j<=r; ++j)
                {
                    MAYBE_INTERRUPT();

                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    ++histogram[iLine[xx]];
                    ++sum;
                }
            }

            // find median
            uint partSum = 0;
            for (uint i=0; i<0x100;++i)
            {
                partSum += histogram.at(i);
                if (partSum>=sum/2)
                {
                    line[x] = i;
                    break;
                }
            }
        }
    }

    return output.convertToFormat(image.format());
}

/*!
    \internal

    when the radius is small, and the orgin image is not grayscale.
 */
static QImage medianFilter_ColorSmall(const QImage& image, uint radius)
{
    QImage output(image.size(),QImage::Format_RGB32);
    const QImage input = image.convertToFormat(QImage::Format_RGB32);
    QVector<QRgb> medianCandidate;
    medianCandidate.reserve(((2*radius+1)*(2*radius+1))); // almost

    const int width = image.width();
    const int height = image.height();
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
                    MAYBE_INTERRUPT();

                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    medianCandidate.append(iLine[xx]);
                }
            }

            // find median
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

    return output.convertToFormat(image.format());
}

/*!
    \internal

    when the radius is large, and the orgin image is not grayscale.
 */
static QImage medianFilter_ColorLarge(const QImage& image, uint radius)
{
    QImage output(image.size(),QImage::Format_RGB32);
    const QImage input = image.convertToFormat(QImage::Format_RGB32);
    QVector<QVector<QRgb>> histogram(0x100);
    for (auto& pixels : histogram)
    {
        pixels.reserve(2*radius*radius); // approximation
    }

    const int width = image.width();
    const int height = image.height();
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
                    MAYBE_INTERRUPT();

                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    histogram[qGray(iLine[xx])].append(iLine[xx]);
                    ++sum;
                }
            }

            // find median
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

    return output.convertToFormat(image.format());
}

/*!
    Filter \a image by replacing every value by the median in its range \a radius neighborhood.
 */
QImage medianFilter(const QImage& image, uint radius)
{
    return image.isGrayscale()
            ? medianFilter_Grayscale(image,radius)
            : radius<10
              ? medianFilter_ColorSmall(image,radius)
              : medianFilter_ColorLarge(image,radius);
}

/*!
    \internal
 */
static inline qreal colorDistance(QRgb a, QRgb b)
{
    return ::std::abs(qGray(a)-qGray(b))/qreal(0xff);
}

/*!
    \internal
 */
static QImage meanShiftFilter_Impl(const QImage& image, uint spatialRadius, qreal colorRadius)
{
    Q_ASSUME(image.format()==QImage::Format_RGB32);

    QImage output(image.size(),QImage::Format_RGB32);

    const int width = image.width();
    const int height = image.height();
    const int r = spatialRadius;

    for (int y=0; y<height; ++y)
    {
        QRgb* line = reinterpret_cast<QRgb*>(output.scanLine(y));
        const QRgb* iLineCenter = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x=0; x<width; ++x)
        {
            int rr=0, gg=0, bb=0;
            int sum=0;
            for (int i=-r; i<=r; ++i)
            {
                const int yy = y+i;
                if (yy<0 || yy>=height)
                    continue;
                const QRgb* iLine = reinterpret_cast<const QRgb*>(image.constScanLine(yy));
                for (int j=-r; j<=r; ++j)
                {
                    MAYBE_INTERRUPT();

                    const int xx = x+j;
                    if (xx<0 || xx>=width)
                        continue;
                    if (colorDistance(iLineCenter[x],iLine[xx]) > colorRadius)
                        continue;
                    rr += qRed(iLine[xx]);
                    gg += qGreen(iLine[xx]);
                    bb += qBlue(iLine[xx]);
                    ++sum;
                }
            }
            line[x] = qRgb(rr/sum,gg/sum,bb/sum);
        }
    }

    return output;
}

/*!
    Filter \a image by replacing every value by the mean of the pixels
    in a range \a spatialRadius neighborhood and whose value is within \a colorRadius.
 */
QImage meanShiftFilter(const QImage& image, uint spatialRadius, qreal colorRadius, uint maxLevel)
{
    QImage output = image.convertToFormat(QImage::Format_RGB32);
    for (uint i=0; i<maxLevel; ++i)
    {
        MAYBE_INTERRUPT();

        output = meanShiftFilter_Impl(output,spatialRadius,colorRadius);
    }
    return output.convertToFormat(image.format());
}

} // namespace MEMS
