#include "circlefit.h"
#include <QImage>
#include <QPoint>
#include <QVector>
#include <QColor>
#include <QtDebug>

namespace MEMS {

QVector<QPoint> whitePixelPositions(const QImage& img)
{
    Q_ASSERT_X(img.format()==QImage::Format_Mono || img.format()==QImage::Format_MonoLSB,
               __func__,"Only monochrome image is valid.");
    const int whiteIndex = img.colorTable().first() == QColor(Qt::white).rgba() ? 0 : 1;

    QVector<QPoint> result;
    const int width = img.width();
    const int height = img.height();
    switch (img.format()) {
    case QImage::Format_Mono:
        for (int y=0; y<height; ++y)
        {
            const uchar* line = img.constScanLine(y);
            for (int x=0; x<width; ++x)
            {
                if (((line[x>>3] >> (7-(x&7))) & 1) == whiteIndex)
                {
                    result.append({x,y});
                }
            }
        }
        break;
    case QImage::Format_MonoLSB:
        for (int y=0; y<height; ++y)
        {
            const uchar* line = img.constScanLine(y);
            for (int x=0; x<width; ++x)
            {
                if (((line[x>>3] >> (x&7)) & 1) == whiteIndex)
                {
                    result.append({x,y});
                }
            }
        }
        break;
    default:
        break;
    }

    return result;
}

} // namespace MEMS
