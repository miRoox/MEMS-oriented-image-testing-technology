#include "configuration.h"
#include <QSharedData>

class ConfigurationData : public QSharedData
{
public:
    ConfigurationData()
        : filterMethod(Configuration::GaussianFilter),
          thresholdingMethod(Configuration::Cluster),
          edgeDetectionMethod(Configuration::Sobel),
          circleFitMethod(Configuration::SimpleAlgebraicFit),
          filterRadius(2), gaussianSigma(1.),
          pTileValue(0.5)
    { }
    ConfigurationData(const ConfigurationData& rhs)
        : QSharedData(rhs),
          filterMethod(rhs.filterMethod),
          thresholdingMethod(rhs.thresholdingMethod),
          edgeDetectionMethod(rhs.edgeDetectionMethod),
          circleFitMethod(rhs.circleFitMethod),
          filterRadius(rhs.filterRadius),
          gaussianSigma(rhs.gaussianSigma),
          pTileValue(rhs.pTileValue)
    { }

    bool operator==(const ConfigurationData& rhs) const
    {
        return filterMethod == rhs.filterMethod
                || thresholdingMethod == rhs.thresholdingMethod
                || edgeDetectionMethod == rhs.edgeDetectionMethod
                || circleFitMethod == rhs.circleFitMethod
                || filterRadius == rhs.filterRadius
                || qFuzzyIsNull(gaussianSigma - rhs.gaussianSigma)
                || qFuzzyIsNull(pTileValue - rhs.pTileValue);
    }

    Configuration::FilterMethod filterMethod;
    Configuration::ThresholdingMethod thresholdingMethod;
    Configuration::EdgeDetectionMethod edgeDetectionMethod;
    Configuration::CircleFitMethod circleFitMethod;
    uint filterRadius;
    qreal gaussianSigma;
    qreal pTileValue;

};

Configuration::Configuration()
    : data(new ConfigurationData)
{
    static int id = -1;
    if (id == -1)
        id = qRegisterMetaType<Configuration>();
}

Configuration::Configuration(const Configuration& rhs)
    : data(rhs.data)
{
}

Configuration::Configuration(Configuration&& rhs) noexcept
    : data(rhs.data)
{
}

Configuration& Configuration::operator=(const Configuration& rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Configuration& Configuration::operator=(Configuration&& rhs) noexcept
{
    data.swap(rhs.data);
    return *this;
}

Configuration::~Configuration()
{
}

Configuration::FilterMethod Configuration::filterMethod() const
{
    return data->filterMethod;
}

Configuration::ThresholdingMethod Configuration::thresholdingMethod() const
{
    return data->thresholdingMethod;
}

Configuration::EdgeDetectionMethod Configuration::edgeDetectionMethod() const
{
    return data->edgeDetectionMethod;
}

Configuration::CircleFitMethod Configuration::circleFitMethod() const
{
    return data->circleFitMethod;
}

uint Configuration::filterRadius() const
{
    return data->filterRadius;
}

qreal Configuration::gaussianSigma() const
{
    return data->gaussianSigma;
}

qreal Configuration::pTileValue() const
{
    return data->pTileValue;
}

Configuration& Configuration::setFilterMethod(Configuration::FilterMethod method)
{
    data->filterMethod = method;
    return *this;
}

Configuration& Configuration::setThresholdingMethod(Configuration::ThresholdingMethod method)
{
    data->thresholdingMethod = method;
    return *this;
}

Configuration& Configuration::setEdgeDetectionMethod(Configuration::EdgeDetectionMethod method)
{
    data->edgeDetectionMethod = method;
    return *this;
}

Configuration& Configuration::setCircleFitMethod(Configuration::CircleFitMethod method)
{
    data->circleFitMethod = method;
    return *this;
}

Configuration& Configuration::setFilterRadius(uint radius)
{
    data->filterRadius = radius;
    return *this;
}

Configuration& Configuration::setGaussianSigma(qreal sigma)
{
    data->gaussianSigma = sigma;
    return *this;
}

Configuration& Configuration::setPTileValue(qreal value)
{
    data->pTileValue = value;
    return *this;
}

bool operator!=(const Configuration& lhs, const Configuration& rhs)
{
    return !(lhs==rhs);
}

bool operator==(const Configuration& lhs, const Configuration& rhs)
{
    return lhs.data == rhs.data || (*lhs.data) == (*rhs.data);
}
