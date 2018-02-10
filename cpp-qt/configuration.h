#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QSharedDataPointer>
#include <QObject>

class ConfigurationData;

class Configuration
{
    Q_GADGET

public:
    Configuration();
    Configuration(const Configuration& );
    Configuration(Configuration&& ) noexcept;
    Configuration& operator=(const Configuration& );
    Configuration& operator=(Configuration&& ) noexcept;
    ~Configuration();

    enum FilterMethod
    {
        BoxFilter,
        GaussianFilter,
        MedianFilter,
    };
    Q_ENUM(FilterMethod)

    enum ThresholdingMethod
    {
        Cluster,
        Mean,
        Moments,
        Fuzziness,
        PTile,
    };
    Q_ENUM(ThresholdingMethod)

    enum EdgeDetectionMethod
    {
        Sobel,
        Prewitt,
        Scharr,
        Laplacian,
    };
    Q_ENUM(EdgeDetectionMethod)

    enum CircleFitMethod
    {
        NaiveFit,
        SimpleAlgebraicFit,
        HyperAlgebraicFit,
    };
    Q_ENUM(CircleFitMethod)

    FilterMethod filterMethod() const;
    ThresholdingMethod thresholdingMethod() const;
    EdgeDetectionMethod edgeDetectionMethod() const;
    CircleFitMethod circleFitMethod() const;
    uint filterRadius() const;
    qreal gaussianSigma() const;
    qreal pTileValue() const;

    Configuration& setFilterMethod(FilterMethod method);
    Configuration& setThresholdingMethod(ThresholdingMethod method);
    Configuration& setEdgeDetectionMethod(EdgeDetectionMethod method);
    Configuration& setCircleFitMethod(CircleFitMethod method);
    Configuration& setFilterRadius(uint radius);
    Configuration& setGaussianSigma(qreal sigma);
    Configuration& setPTileValue(qreal value);

    friend bool operator!=(const Configuration& lhs, const Configuration& rhs);
    friend bool operator==(const Configuration& lhs, const Configuration& rhs);

private:
    QSharedDataPointer<ConfigurationData> data;
};

#endif // CONFIGURATION_H
