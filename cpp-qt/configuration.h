#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QSharedDataPointer>
#include "processor.h"

class ConfigurationData;

class Configuration
{
public:
    Configuration();
    Configuration(const Configuration& );
    Configuration(Configuration&& ) noexcept;
    Configuration& operator=(const Configuration& );
    Configuration& operator=(Configuration&& ) noexcept;
    ~Configuration();

    Processor::FilterMethod filterMethod() const;
    Processor::ThresholdingMethod thresholdingMethod() const;
    Processor::EdgeDetectionMethod edgeDetectionMethod() const;
    Processor::CircleFitMethod circleFitMethod() const;
    uint filterRadius() const;
    qreal gaussianSigma() const;
    qreal pTileValue() const;

    Configuration& setFilterMethod(Processor::FilterMethod method);
    Configuration& setThresholdingMethod(Processor::ThresholdingMethod method);
    Configuration& setEdgeDetectionMethod(Processor::EdgeDetectionMethod method);
    Configuration& setCircleFitMethod(Processor::CircleFitMethod method);
    Configuration& setFilterRadius(uint radius);
    Configuration& setGaussianSigma(qreal sigma);
    Configuration& setPTileValue(qreal value);

    friend bool operator!=(const Configuration& lhs, const Configuration& rhs);
    friend bool operator==(const Configuration& lhs, const Configuration& rhs);

private:
    QSharedDataPointer<ConfigurationData> data;
};

Q_DECLARE_METATYPE(Configuration)

#endif // CONFIGURATION_H
