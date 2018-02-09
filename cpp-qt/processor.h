#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QScopedPointer>
#include <QImage>
#include <QPointF>

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
    Q_PROPERTY(FilterMethod filterMethod READ filterMethod WRITE setFilterMethod NOTIFY filterMethodChanged)
    Q_PROPERTY(ThresholdingMethod thresholdingMethod READ thresholdingMethod WRITE setThresholdingMethod NOTIFY thresholdingMethodChanged)
    Q_PROPERTY(EdgeDetectionMethod edgeDetectionMethod READ edgeDetectionMethod WRITE setEdgeDetectionMethod NOTIFY edgeDetectionMethodChanged)
    Q_PROPERTY(CircleFitMethod circleFitMethod READ circleFitMethod WRITE setCircleFitMethod NOTIFY circleFitMethodChanged)
    Q_PROPERTY(uint filterRadius READ filterRadius WRITE setFilterRadius NOTIFY filterRadiusChanged)
    Q_PROPERTY(qreal gaussianSigma READ gaussianSigma WRITE setGaussianSigma NOTIFY gaussianSigmaChanged)
    Q_PROPERTY(qreal pTileValue READ pTileValue WRITE setPTileValue NOTIFY pTileValueChanged)
    Q_PROPERTY(int threshold READ threshold NOTIFY thresholdChanged)

public:
    explicit Processor(QObject* parent = nullptr);
    ~Processor();

    QImage originImage() const;
    QImage filteredImage() const;
    QImage binaryImage() const;
    QImage edgeImage() const;
    QImage circleImage() const;
    QPointF circleCenter() const;
    qreal circleRadius() const;

    enum FilterMethod
    {
        BoxFilter,
        GaussianFilter,
        MedianFilter,
    };
    Q_ENUM(FilterMethod)

    FilterMethod filterMethod() const;
    uint filterRadius() const;
    qreal gaussianSigma() const;

    enum ThresholdingMethod
    {
        Cluster,
        Mean,
        Moments,
        Fuzziness,
        PTile,
    };
    Q_ENUM(ThresholdingMethod)

    ThresholdingMethod thresholdingMethod() const;
    qreal pTileValue() const;
    int threshold() const;

    enum EdgeDetectionMethod
    {
        Sobel,
        Prewitt,
        Scharr,
        Laplacian,
    };
    Q_ENUM(EdgeDetectionMethod)

    EdgeDetectionMethod edgeDetectionMethod() const;

    enum CircleFitMethod
    {
        NaiveFit,
        SimpleAlgebraicFit,
        HyperAlgebraicFit,
    };
    Q_ENUM(CircleFitMethod)

    CircleFitMethod circleFitMethod() const;

signals:
    void originImageChanged(const QImage& origin);
    void filteredImageChanged(const QImage& filtered);
    void binaryImageChanged(const QImage& binary);
    void edgeImageChanged(const QImage& edge);
    void circleImageChanged(const QImage& circle);
    void circleCenterChanged(const QPointF& center);
    void circleRadiusChanged(qreal radius);
    void filterMethodChanged(FilterMethod method);
    void thresholdingMethodChanged(ThresholdingMethod method);
    void edgeDetectionMethodChanged(EdgeDetectionMethod method);
    void circleFitMethodChanged(CircleFitMethod method);
    void filterRadiusChanged(uint radius);
    void gaussianSigmaChanged(qreal sigma);
    void pTileValueChanged(qreal value);
    void thresholdChanged(int threshold);

public slots:
    void setOriginImage(const QImage& origin);
    void setFilterMethod(FilterMethod method);
    void setThresholdingMethod(ThresholdingMethod method);
    void setEdgeDetectionMethod(EdgeDetectionMethod method);
    void setCircleFitMethod(CircleFitMethod method);
    void setFilterRadius(uint radius);
    void setGaussianSigma(qreal sigma);
    void setPTileValue(qreal value);

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
