#include "processor.h"
#include "algorithm.h"
#include <QImage>
#include <QPointer>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QFileInfo>
#include <QtDebug>


/*!
    \internal
 */
class Processor::Impl
{
    QPointer<Processor> q;
public:
    QImage origin;
    QImage filtered;
    QImage binarized;
    QImage edge;
    QImage circle;
    MEMS::CircleData circleData;

    Processor::FilterMethod filterMethod;
    Processor::ThresholdingMethod thresholdingMethod;
    Processor::EdgeDetectionMethod edgeMethod;
    Processor::CircleFitMethod circleFitMethod;

    uint filterRadius;
    qreal gaussianSigma;

    qreal pTileValue;

    MEMS::Histogram filteredHisto;
    int threshold;
    QVector<QPoint> edgePixels;

    Impl(Processor* interface)
        : q(interface),
          filterMethod(Processor::GaussianFilter),
          thresholdingMethod(Processor::Cluster),
          edgeMethod(Processor::Sobel),
          circleFitMethod(Processor::SimpleAlgebraicFit),
          filterRadius(2), gaussianSigma(1.),
          pTileValue(0.5)
    {
    }

    void updateFilteredImage()
    {
        using namespace MEMS;
        if (origin.isNull())
            return;
        switch (filterMethod)
        {
        case Processor::BoxFilter:
            q->setFilteredImage(boxFilter(origin,filterRadius));
            break;
        case Processor::GaussianFilter:
            q->setFilteredImage(gaussianFilter(origin,filterRadius,gaussianSigma));
            break;
        case Processor::MedianFilter:
            q->setFilteredImage(medianFilter(origin,filterRadius));
            break;
        default:
            break;
        }
    }

    void updateThreshold()
    {
        using namespace MEMS;
        if (filtered.isNull() || filteredHisto.isEmpty())
            return;
        switch (thresholdingMethod)
        {
        case Processor::Cluster:
            q->setThreshold(clusterThreshold(filteredHisto));
            break;
        case Processor::Mean:
            q->setThreshold(meanThreshold(filteredHisto));
            break;
        case Processor::Moments:
            q->setThreshold(momentsThreshold(filteredHisto));
            break;
        case Processor::Fuzziness:
            q->setThreshold(fuzzinessThreshold(filteredHisto));
            break;
        case Processor::PTile:
            q->setThreshold(pTileThreshold(filteredHisto,pTileValue));
        default:
            break;
        }
    }

    void updateEdgeImage()
    {
        using namespace MEMS;
        if (binarized.isNull())
            return;
        switch (edgeMethod)
        {
        case Processor::Sobel:
            q->setEdgeImage(sobelOperator(binarized));
            break;
        case Processor::Prewitt:
            q->setEdgeImage(prewittOperator(binarized));
            break;
        case Processor::Scharr:
            q->setEdgeImage(scharrOperator(binarized));
            break;
        case Processor::Laplacian:
            q->setEdgeImage(laplacianOperator(binarized));
            break;
        default:
            break;
        }
    }

    void updateCircle()
    {
        using namespace MEMS;
        if (edge.isNull() || edgePixels.isEmpty())
            return;
        switch (circleFitMethod)
        {
        case Processor::NaiveFit:
            q->setCircle(naiveCircleFit(edgePixels));
            break;
        case Processor::SimpleAlgebraicFit:
            q->setCircle(simpleAlgebraicCircleFit(edgePixels));
            break;
        case Processor::HyperAlgebraicFit:
            q->setCircle(hyperAlgebraicCircleFit(edgePixels));
            break;
        default:
            break;
        }
        QImage copy = origin.convertToFormat(QImage::Format_RGB32);
        QPainter painter(&copy);
        QPen pen(Qt::red);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawEllipse(circleData.center,circleData.radius,circleData.radius);
        painter.drawText(4,4,copy.width()/8.,copy.height()/8.,Qt::AlignLeft | Qt::TextDontClip,
                         q->tr("Center: (%1, %2)\n"
                               "Radius: %3")
                         .arg(circleData.center.x()).arg(circleData.center.y())
                         .arg(circleData.radius));
        q->setCircleImage(copy);
    }

}; // class Processor::Impl

Processor::Processor(QObject* parent)
    : QObject(parent), d(new Impl(this))
{
}

Processor::~Processor()
{
}

QImage Processor::originImage() const
{
    return d->origin;
}

void Processor::setOriginImage(const QImage& origin)
{
    if (origin.isNull())
        return;
    if (d->origin == origin)
        return;
    d->origin = origin;
    emit originImageChanged(d->origin);

    d->updateFilteredImage();
}

QImage Processor::filteredImage() const
{
    return d->filtered;
}

void Processor::setFilteredImage(const QImage& filtered)
{
    if (filtered.isNull())
        return;
    if (d->filtered == filtered)
        return;
    d->filtered = filtered;
    emit filteredImageChanged(d->filtered);

    d->filteredHisto = MEMS::grayscaleHistogram(d->filtered);
    d->updateThreshold();
    setBinaryImage(MEMS::binarize(d->filtered,d->threshold));
}

QImage Processor::binaryImage() const
{
    return d->binarized;
}

void Processor::setBinaryImage(const QImage& binary)
{
    if (binary.isNull())
        return;
    if (d->binarized == binary)
        return;
    d->binarized = binary;
    emit binaryImageChanged(d->binarized);

    d->updateEdgeImage();
}

QImage Processor::edgeImage() const
{
    return d->edge;
}

void Processor::setEdgeImage(const QImage& edge)
{
    if (edge.isNull())
        return;
    if (d->edge == edge)
        return;
    d->edge = edge;
    emit edgeImageChanged(d->edge);

    d->edgePixels = MEMS::whitePixelPositions(d->edge);
    d->updateCircle();
}

QImage Processor::circleImage() const
{
    return d->circle;
}

void Processor::setCircleImage(const QImage& circle)
{
    if (circle.isNull())
        return;
    if (d->circle == circle)
        return;
    d->circle = circle;
    emit circleImageChanged(d->circle);
}

QPointF Processor::circleCenter() const
{
    return d->circleData.center;
}

qreal Processor::circleRadius() const
{
    return d->circleData.radius;
}

void Processor::setCircle(const MEMS::CircleData& circle)
{
    if (d->circleData == circle)
        return;
    d->circleData = circle;
    emit circleCenterChanged(d->circleData.center);
    emit circleRadiusChanged(d->circleData.radius);

    qInfo() << "The center of the circle is ("
            << d->circleData.center.x() << ", "
            << d->circleData.center.y() << ") ,"
            << "and the radius is " << d->circleData.radius;
}

Processor::FilterMethod Processor::filterMethod() const
{
    return d->filterMethod;
}

void Processor::setFilterMethod(Processor::FilterMethod method)
{
    if (d->filterMethod == method)
        return;
    d->filterMethod = method;
    emit filterMethodChanged(d->filterMethod);

    d->updateFilteredImage();
}

Processor::ThresholdingMethod Processor::thresholdingMethod() const
{
    return d->thresholdingMethod;
}

void Processor::setThresholdingMethod(ThresholdingMethod method)
{
    if (d->thresholdingMethod == method)
        return;
    d->thresholdingMethod = method;
    emit thresholdingMethodChanged(d->thresholdingMethod);

    d->updateThreshold();
}

Processor::EdgeDetectionMethod Processor::edgeDetectionMethod() const
{
    return d->edgeMethod;
}

void Processor::setEdgeDetectionMethod(Processor::EdgeDetectionMethod method)
{
    if (d->edgeMethod == method)
        return;
    d->edgeMethod = method;
    emit edgeDetectionMethodChanged(d->edgeMethod);

    d->updateEdgeImage();
}

Processor::CircleFitMethod Processor::circleFitMethod() const
{
    return d->circleFitMethod;
}

void Processor::setCircleFitMethod(Processor::CircleFitMethod method)
{
    if (d->circleFitMethod == method)
        return;
    d->circleFitMethod = method;
    emit circleFitMethodChanged(d->circleFitMethod);

    d->updateCircle();
}

uint Processor::filterRadius() const
{
    return d->filterRadius;
}

void Processor::setFilterRadius(uint radius)
{
    if (d->filterRadius == radius)
        return;
    d->filterRadius = radius;
    emit filterRadiusChanged(d->filterRadius);

    d->updateFilteredImage();
}

qreal Processor::gaussianSigma() const
{
    return d->gaussianSigma;
}

void Processor::setGaussianSigma(qreal sigma)
{
    if (d->filterMethod != Processor::GaussianFilter)
        return;
    if (qFuzzyIsNull(d->gaussianSigma - sigma))
        return;
    d->gaussianSigma = sigma;
    emit gaussianSigmaChanged(d->gaussianSigma);

    d->updateFilteredImage();
}

qreal Processor::pTileValue() const
{
    return d->pTileValue;
}

void Processor::setPTileValue(qreal value)
{
    if (d->thresholdingMethod != Processor::PTile)
        return;
    if (qFuzzyIsNull(d->pTileValue - value))
        return;
    d->pTileValue = value;
    emit pTileValueChanged(d->pTileValue);

    d->updateThreshold();
}

int Processor::threshold() const
{
    return d->threshold;
}

void Processor::setThreshold(int threshold)
{
    if (d->threshold == threshold)
        return;
    d->threshold = threshold;
    emit thresholdChanged(d->threshold);

    setBinaryImage(MEMS::binarize(d->filtered,d->threshold));
}

void Processor::exportResult(const QString& fileName) const
{
    if (d->circle.save(fileName))
    {
        qInfo() << "Export file"
                << QFileInfo(fileName).canonicalFilePath()
                << "successfully";
    }
    else
    {
        qWarning() << "Export file" << fileName << "failed";
    }
}
