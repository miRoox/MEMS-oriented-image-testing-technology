#include "imagefilter.h"
#include <QImage>
#include <QPoint>
#include <QColor>

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

QImage boxFilter(const QImage& origin, uint radius, PaddingType padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

extern QImage boxFilter(const QImage& origin, uint radius, QRgb padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

extern QImage boxFilter(const QImage& origin, uint radius, const QColor& padding)
{
    auto boxKer = QVector<QVector<qreal>>(2*radius+1,QVector<qreal>(2*radius+1,
                                                         1./(2*radius+1)/(2*radius+1)));
    return convolve(origin,boxKer,padding);
}

} // namespace MEMS
