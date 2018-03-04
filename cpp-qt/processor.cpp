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


#include "processor.h"
#include "algorithms.h"
#include <QImage>
#include <QPointer>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QFileInfo>
#include <utils.h>

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

    Configuration::FilterMethod filterMethod;
    Configuration::ThresholdingMethod thresholdingMethod;
    Configuration::EdgeDetectionMethod edgeMethod;
    Configuration::CircleFitMethod circleFitMethod;
    Configuration::ErrorCorrectionMethod errorCorrectionMethod;

    uint filterRadius;
    qreal gaussianSigma;
    qreal colorRadius;
    uint maxLevel;

    qreal pTileValue;

    MEMS::Histogram filteredHisto;
    int threshold;
    QVector<QPoint> edgePixels;
    bool lazy = true;

    Impl(Processor* interface, const Configuration& config)
        : q(interface),
          filterMethod(config.filterMethod()),
          thresholdingMethod(config.thresholdingMethod()),
          edgeMethod(config.edgeDetectionMethod()),
          circleFitMethod(config.circleFitMethod()),
          errorCorrectionMethod(config.errorCorrectionMethod()),
          filterRadius(config.filterRadius()),
          gaussianSigma(config.gaussianSigma()),
          colorRadius(config.colorRadius()),
          maxLevel(config.maxLevel()),
          pTileValue(config.pTileValue())
    {
    }

    void updateFilteredImage()
    {
        using namespace MEMS;
        if (lazy)
            return;
        if (origin.isNull())
            return;
        switch (filterMethod)
        {
        case Configuration::BoxFilter:
            q->setFilteredImage(TIMING(boxFilter(origin,filterRadius)));
            break;
        case Configuration::GaussianFilter:
            q->setFilteredImage(TIMING(gaussianFilter(origin,filterRadius,gaussianSigma)));
            break;
        case Configuration::MedianFilter:
            q->setFilteredImage(TIMING(medianFilter(origin,filterRadius)));
            break;
        case Configuration::MeanShiftFilter:
            q->setFilteredImage(TIMING(meanShiftFilter(origin,filterRadius,colorRadius,maxLevel)));
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    void updateThreshold()
    {
        using namespace MEMS;
        if (lazy)
            return;
        if (filtered.isNull() || filteredHisto.isEmpty())
            return;
        switch (thresholdingMethod)
        {
        case Configuration::Cluster:
            q->setThreshold(TIMING(clusterThreshold(filteredHisto)));
            break;
        case Configuration::Mean:
            q->setThreshold(TIMING(meanThreshold(filteredHisto)));
            break;
        case Configuration::Moments:
            q->setThreshold(TIMING(momentsThreshold(filteredHisto)));
            break;
        case Configuration::Fuzziness:
            q->setThreshold(TIMING(fuzzinessThreshold(filteredHisto)));
            break;
        case Configuration::PTile:
            q->setThreshold(TIMING(pTileThreshold(filteredHisto,pTileValue)));
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    void updateEdgeImage()
    {
        using namespace MEMS;
        if (lazy)
            return;
        if (binarized.isNull())
            return;
        switch (edgeMethod)
        {
        case Configuration::Sobel:
            q->setEdgeImage(TIMING(sobelOperator(binarized)));
            break;
        case Configuration::Prewitt:
            q->setEdgeImage(TIMING(prewittOperator(binarized)));
            break;
        case Configuration::Scharr:
            q->setEdgeImage(TIMING(scharrOperator(binarized)));
            break;
        case Configuration::Laplacian:
            q->setEdgeImage(TIMING(laplacianOperator(binarized)));
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
    }

    void updateCircle()
    {
        using namespace MEMS;
        if (lazy)
            return;
        if (edge.isNull() || edgePixels.isEmpty())
            return;
        CircleFitFunction fit = nullptr;
        switch (circleFitMethod)
        {
        case Configuration::NaiveFit:
            fit = naiveCircleFit;
            break;
        case Configuration::SimpleAlgebraicFit:
            fit = simpleAlgebraicCircleFit;
            break;
        case Configuration::HyperAlgebraicFit:
            fit = hyperAlgebraicCircleFit;
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        switch (errorCorrectionMethod)
        {
        case Configuration::NoCorrection:
            q->setCircle(TIMING(noCorrection(fit,edgePixels)));
            break;
        case Configuration::MedianError:
            q->setCircle(TIMING(medianErrorCorrection(fit,edgePixels)));
            break;
        case Configuration::ConnectivityBased:
            q->setCircle(TIMING(connectivityBasedCorrection(fit,edgePixels)));
            break;
        default:
            Q_UNREACHABLE();
            break;
        }
        QImage copy = origin.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&copy);
        QPen pen(Qt::red);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawEllipse(circleData.center,circleData.radius,circleData.radius);
        QFont font = painter.font();
        font.setPointSize(2*font.pointSize());
        painter.setFont(font);
        painter.drawText(4,4,copy.width()/8.,copy.height()/8.,Qt::AlignLeft | Qt::TextDontClip,
                         Processor::tr("Center: (%1, %2)\n"
                                       "Radius: %3")
                         .arg(circleData.center.x()).arg(circleData.center.y())
                         .arg(circleData.radius));
        q->setCircleImage(copy);
    }

}; // class Processor::Impl

Processor::Processor(QObject* parent)
    : QObject(parent), d(new Impl(this,Configuration()))
{
}

Processor::Processor(const Configuration& config, QObject* parent)
    : QObject(parent), d(new Impl(this,config))
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

    d->lazy = false;
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
    if (circle.isNull())
        return;
    if (d->circleData == circle)
        return;
    d->circleData = circle;
    emit circleCenterChanged(d->circleData.center);
    emit circleRadiusChanged(d->circleData.radius);

    qDebug() << "The center of the circle is" << d->circleData.center
             << "and the radius is" << d->circleData.radius
             << ", with the" << configurations();
}

Configuration Processor::configurations() const
{
    return Configuration()
            .setFilterMethod(d->filterMethod)
            .setThresholdingMethod(d->thresholdingMethod)
            .setEdgeDetectionMethod(d->edgeMethod)
            .setCircleFitMethod(d->circleFitMethod)
            .setErrorCorrectionMethod(d->errorCorrectionMethod)
            .setFilterRadius(d->filterRadius)
            .setGaussianSigma(d->gaussianSigma)
            .setColorRadius(d->colorRadius)
            .setMaxLevel(d->maxLevel)
            .setPTileValue(d->pTileValue);
}

void Processor::setConfigurations(const Configuration& config)
{
    d->lazy = true;
    setFilterMethod(config.filterMethod());
    setFilterRadius(config.filterRadius());
    setGaussianSigma(config.gaussianSigma());
    setColorRadius(config.colorRadius());
    setMaxLevel(config.maxLevel());
    setThresholdingMethod(config.thresholdingMethod());
    setPTileValue(config.pTileValue());
    setEdgeDetectionMethod(config.edgeDetectionMethod());
    setCircleFitMethod(config.circleFitMethod());
    setErrorCorrectionMethod(config.errorCorrectionMethod());

    d->lazy = false;
    d->updateFilteredImage();
    d->updateThreshold();
    d->updateEdgeImage();
    d->updateCircle();
}

Configuration::FilterMethod Processor::filterMethod() const
{
    return d->filterMethod;
}

void Processor::setFilterMethod(Configuration::FilterMethod method)
{
    if (d->filterMethod == method)
        return;
    d->filterMethod = method;
    emit filterMethodChanged(d->filterMethod);

    d->updateFilteredImage();
}

Configuration::ThresholdingMethod Processor::thresholdingMethod() const
{
    return d->thresholdingMethod;
}

void Processor::setThresholdingMethod(Configuration::ThresholdingMethod method)
{
    if (d->thresholdingMethod == method)
        return;
    d->thresholdingMethod = method;
    emit thresholdingMethodChanged(d->thresholdingMethod);

    d->updateThreshold();
}

Configuration::EdgeDetectionMethod Processor::edgeDetectionMethod() const
{
    return d->edgeMethod;
}

void Processor::setEdgeDetectionMethod(Configuration::EdgeDetectionMethod method)
{
    if (d->edgeMethod == method)
        return;
    d->edgeMethod = method;
    emit edgeDetectionMethodChanged(d->edgeMethod);

    d->updateEdgeImage();
}

Configuration::CircleFitMethod Processor::circleFitMethod() const
{
    return d->circleFitMethod;
}

void Processor::setCircleFitMethod(Configuration::CircleFitMethod method)
{
    if (d->circleFitMethod == method)
        return;
    d->circleFitMethod = method;
    emit circleFitMethodChanged(d->circleFitMethod);

    d->updateCircle();
}

Configuration::ErrorCorrectionMethod Processor::errorCorrectionMethod() const
{
    return d->errorCorrectionMethod;
}

void Processor::setErrorCorrectionMethod(Configuration::ErrorCorrectionMethod method)
{
    if (d->errorCorrectionMethod == method)
        return;
    d->errorCorrectionMethod = method;
    emit errorCorrectionMethodChanged(d->errorCorrectionMethod);

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
    if (qFuzzyIsNull(d->gaussianSigma - sigma))
        return;
    d->gaussianSigma = sigma;
    emit gaussianSigmaChanged(d->gaussianSigma);

    d->updateFilteredImage();
}

qreal Processor::colorRadius() const
{
    return d->colorRadius;
}

void Processor::setColorRadius(qreal radius)
{
    if (qFuzzyIsNull(d->colorRadius - radius))
        return;
    d->colorRadius = radius;
    emit colorRadiusChanged(d->colorRadius);

    d->updateFilteredImage();
}

uint Processor::maxLevel() const
{
    return d->maxLevel;
}

void Processor::setMaxLevel(uint level)
{
    if (d->maxLevel == level)
        return;
    d->maxLevel = level;
    emit maxLevelChanged(d->maxLevel);

    d->updateFilteredImage();
}

qreal Processor::pTileValue() const
{
    return d->pTileValue;
}

void Processor::setPTileValue(qreal value)
{
    if (qFuzzyIsNull(d->pTileValue - value))
        return;
    d->pTileValue = value;
    emit pTileValueChanged(d->pTileValue);

    d->updateThreshold();
}

void Processor::saveConfigurations(const QString& group) const
{
    Configuration config = configurations();
    saveConfigs(config,group);
    qInfo() << "For" << qPrintable(group) << ", save current" << config;
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
