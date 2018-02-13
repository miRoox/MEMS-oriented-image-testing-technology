#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QScopedPointer>
#include <QImage>
#include <QPointF>
#include "configuration.h"

class QString;

namespace MEMS {
struct CircleData;
}

class Processor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage originImage READ originImage WRITE setOriginImage NOTIFY originImageChanged)
    Q_PROPERTY(QImage filteredImage READ filteredImage NOTIFY filteredImageChanged)
    Q_PROPERTY(QImage binaryImage READ binaryImage NOTIFY binaryImageChanged)
    Q_PROPERTY(QImage edgeImage READ edgeImage NOTIFY edgeImageChanged)
    Q_PROPERTY(QImage circleImage READ circleImage NOTIFY circleImageChanged)
    Q_PROPERTY(QPointF circleCenter READ circleCenter NOTIFY circleCenterChanged)
    Q_PROPERTY(qreal circleRadius READ circleRadius NOTIFY circleRadiusChanged)
    Q_PROPERTY(Configuration configurations READ configurations WRITE setConfigurations STORED false)
    Q_PROPERTY(Configuration::FilterMethod filterMethod READ filterMethod WRITE setFilterMethod NOTIFY filterMethodChanged)
    Q_PROPERTY(Configuration::ThresholdingMethod thresholdingMethod READ thresholdingMethod WRITE setThresholdingMethod NOTIFY thresholdingMethodChanged)
    Q_PROPERTY(Configuration::EdgeDetectionMethod edgeDetectionMethod READ edgeDetectionMethod WRITE setEdgeDetectionMethod NOTIFY edgeDetectionMethodChanged)
    Q_PROPERTY(Configuration::CircleFitMethod circleFitMethod READ circleFitMethod WRITE setCircleFitMethod NOTIFY circleFitMethodChanged)
    Q_PROPERTY(uint filterRadius READ filterRadius WRITE setFilterRadius NOTIFY filterRadiusChanged)
    Q_PROPERTY(qreal gaussianSigma READ gaussianSigma WRITE setGaussianSigma NOTIFY gaussianSigmaChanged)
    Q_PROPERTY(qreal pTileValue READ pTileValue WRITE setPTileValue NOTIFY pTileValueChanged)
    Q_PROPERTY(int threshold READ threshold NOTIFY thresholdChanged)

public:
    explicit Processor(QObject* parent = nullptr);
    explicit Processor(const Configuration& config, QObject* parent = nullptr);
    ~Processor();

    QImage originImage() const;
    QImage filteredImage() const;
    QImage binaryImage() const;
    QImage edgeImage() const;
    QImage circleImage() const;
    QPointF circleCenter() const;
    qreal circleRadius() const;

    Configuration configurations() const;

    Configuration::FilterMethod filterMethod() const;
    uint filterRadius() const;
    qreal gaussianSigma() const;

    Configuration::ThresholdingMethod thresholdingMethod() const;
    qreal pTileValue() const;
    int threshold() const;

    Configuration::EdgeDetectionMethod edgeDetectionMethod() const;

    Configuration::CircleFitMethod circleFitMethod() const;



signals:
    void originImageChanged(const QImage& origin);
    void filteredImageChanged(const QImage& filtered);
    void binaryImageChanged(const QImage& binary);
    void edgeImageChanged(const QImage& edge);
    void circleImageChanged(const QImage& circle);
    void circleCenterChanged(const QPointF& center);
    void circleRadiusChanged(qreal radius);
    void filterMethodChanged(Configuration::FilterMethod method);
    void thresholdingMethodChanged(Configuration::Configuration::ThresholdingMethod method);
    void edgeDetectionMethodChanged(Configuration::EdgeDetectionMethod method);
    void circleFitMethodChanged(Configuration::CircleFitMethod method);
    void filterRadiusChanged(uint radius);
    void gaussianSigmaChanged(qreal sigma);
    void pTileValueChanged(qreal value);
    void thresholdChanged(int threshold);

public slots:
    void setOriginImage(const QImage& origin);
    void setConfigurations(const Configuration& config);
    void setFilterMethod(Configuration::FilterMethod method);
    void setThresholdingMethod(Configuration::ThresholdingMethod method);
    void setEdgeDetectionMethod(Configuration::EdgeDetectionMethod method);
    void setCircleFitMethod(Configuration::CircleFitMethod method);
    void setFilterRadius(uint radius);
    void setGaussianSigma(qreal sigma);
    void setPTileValue(qreal value);

    void saveConfigurations(const QString& group) const;
    void exportResult(const QString& fileName) const;

private slots: // internal
    void setFilteredImage(const QImage& filtered);
    void setBinaryImage(const QImage& binary);
    void setEdgeImage(const QImage& edge);
    void setCircleImage(const QImage& circle);
    void setCircle(const MEMS::CircleData& circle);
    void setThreshold(int threshold);

private:
    class Impl;
    QScopedPointer<Impl> d;

    Q_DISABLE_COPY(Processor)

    friend class Impl;

}; // class Processor

#endif // PROCESSOR_H
