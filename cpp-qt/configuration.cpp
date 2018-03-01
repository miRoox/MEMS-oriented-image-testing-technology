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


#include "configuration.h"
#include <QSharedData>
#include <QSettings>
#include <QVariant>
#include <QMetaEnum>
#include <QApplication>
#include <QtDebug>

static constexpr auto DefaultFilterMethod = Configuration::GaussianFilter;
static constexpr auto DefaultThresholdingMethod = Configuration::Cluster;
static constexpr auto DefaultEdgeDetectionMethod = Configuration::Sobel;
static constexpr auto DefaultCircleFitMethod = Configuration::SimpleAlgebraicFit;
static constexpr auto DefaultErrorCorrectionMethod = Configuration::NoCorrection;
static constexpr auto DefaultFilterRadius = 2u;
static constexpr auto DefaultGaussianSigma = 1.;
static constexpr auto DefaultColorRadius = 0.1;
static constexpr auto DefaultMaxLevel = 1u;
static constexpr auto DefaultPTileValue = 0.5;

/*!
    \internal
 */
class ConfigurationData : public QSharedData
{
public:
    ConfigurationData()
    { }
    ConfigurationData(const ConfigurationData& rhs)
        : QSharedData(rhs),
          filterMethod(rhs.filterMethod),
          thresholdingMethod(rhs.thresholdingMethod),
          edgeDetectionMethod(rhs.edgeDetectionMethod),
          circleFitMethod(rhs.circleFitMethod),
          filterRadius(rhs.filterRadius),
          gaussianSigma(rhs.gaussianSigma),
          colorRadius(rhs.colorRadius),
          maxLevel(rhs.maxLevel),
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
                || qFuzzyIsNull(colorRadius - rhs.colorRadius)
                || maxLevel == rhs.maxLevel
                || qFuzzyIsNull(pTileValue - rhs.pTileValue);
    }

    Configuration::FilterMethod filterMethod = DefaultFilterMethod;
    Configuration::ThresholdingMethod thresholdingMethod = DefaultThresholdingMethod;
    Configuration::EdgeDetectionMethod edgeDetectionMethod = DefaultEdgeDetectionMethod;
    Configuration::CircleFitMethod circleFitMethod = DefaultCircleFitMethod;
    Configuration::ErrorCorrectionMethod errorCorrectionMethod = DefaultErrorCorrectionMethod;
    uint filterRadius = DefaultFilterRadius;
    qreal gaussianSigma = DefaultGaussianSigma;
    qreal colorRadius = DefaultColorRadius;
    qreal maxLevel = DefaultMaxLevel;
    qreal pTileValue = DefaultPTileValue;

};

Configuration::Configuration()
    : data(new ConfigurationData)
{
    static int id = -1;
    if (id == -1)
    {
        id = qRegisterMetaType<Configuration>();
        qRegisterMetaType<Configuration::FilterMethod>();
        qRegisterMetaType<Configuration::ThresholdingMethod>();
        qRegisterMetaType<Configuration::EdgeDetectionMethod>();
        qRegisterMetaType<Configuration::CircleFitMethod>();
        qRegisterMetaType<Configuration::ErrorCorrectionMethod>();
    }
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

Configuration::ErrorCorrectionMethod Configuration::errorCorrectionMethod() const
{
    return data->errorCorrectionMethod;
}

uint Configuration::filterRadius() const
{
    return data->filterRadius;
}

qreal Configuration::gaussianSigma() const
{
    return data->gaussianSigma;
}

qreal Configuration::colorRadius() const
{
    return data->colorRadius;
}

uint Configuration::maxLevel() const
{
    return data->maxLevel;
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

Configuration& Configuration::setErrorCorrectionMethod(Configuration::ErrorCorrectionMethod method)
{
    data->errorCorrectionMethod = method;
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

Configuration& Configuration::setColorRadius(qreal radius)
{
    data->colorRadius = radius;
    return *this;
}

Configuration& Configuration::setMaxLevel(uint level)
{
    data->maxLevel = level;
    return *this;
}

Configuration& Configuration::setPTileValue(qreal value)
{
    data->pTileValue = value;
    return *this;
}

Configuration::FilterMethod Configuration::defaultFilterMethod()
{
    return DefaultFilterMethod;
}

Configuration::ThresholdingMethod Configuration::defaultThresholdingMethod()
{
    return DefaultThresholdingMethod;
}

Configuration::EdgeDetectionMethod Configuration::defaultEdgeDetectionMethod()
{
    return DefaultEdgeDetectionMethod;
}

Configuration::CircleFitMethod Configuration::defaultCircleFitMethod()
{
    return DefaultCircleFitMethod;
}

Configuration::ErrorCorrectionMethod Configuration::defaultErrorCorrectionMethod()
{
    return DefaultErrorCorrectionMethod;
}

uint Configuration::defaultFilterRadius()
{
    return DefaultFilterRadius;
}

qreal Configuration::defaultGaussianSigma()
{
    return DefaultGaussianSigma;
}

qreal Configuration::defaultColorRadius()
{
    return DefaultColorRadius;
}

qreal Configuration::defaultMaxLevel()
{
    return DefaultMaxLevel;
}

qreal Configuration::defaultPTileValue()
{
    return DefaultPTileValue;
}

/** related non-member **/

bool operator!=(const Configuration& lhs, const Configuration& rhs)
{
    return !(lhs==rhs);
}

bool operator==(const Configuration& lhs, const Configuration& rhs)
{
    return lhs.data == rhs.data || (*lhs.data) == (*rhs.data);
}

/*!
    \internal
 */
template<typename Enum>
static Enum keyToValue(const QString& key, Enum defaultValue = static_cast<Enum>(0))
{
    static QMetaEnum meta = QMetaEnum::fromType<Enum>();
    bool ok = false;
    Enum value = static_cast<Enum>(meta.keyToValue(key.toLatin1(),&ok));
    if (!ok)
        value = defaultValue;
    return value;
}

/*!
    \internal
 */
template<typename Enum>
static QString valueToKey(Enum value)
{
    static QMetaEnum meta = QMetaEnum::fromType<Enum>();
    return meta.valueToKey(value);
}

QDebug operator<<(QDebug dbg, const Configuration& config)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "Configuration("
                  << "FilterMethod: " <<  qPrintable(valueToKey(config.data->filterMethod)) << ", "
                  << "ThresholdingMethod: " << qPrintable(valueToKey(config.data->thresholdingMethod)) << ", "
                  << "EdgeDetectionMethod: " << qPrintable(valueToKey(config.data->edgeDetectionMethod)) << ", "
                  << "CircleFitMethod: " << qPrintable(valueToKey(config.data->circleFitMethod)) << ", "
                  << "ErrorCorrectionMethod: " << qPrintable(valueToKey(config.data->errorCorrectionMethod)) << ", "
                  << "FilterRadius: " << config.data->filterRadius << ", "
                  << "GaussianSigma: " << config.data->gaussianSigma << ", "
                  << "ColorRadius: " << config.data->colorRadius << ", "
                  << "MaxLevel: " << config.data->maxLevel << ", "
                  << "PTileValue: " << config.data->pTileValue << ")";

    return dbg;
}

static constexpr const char FilterMethodKey[] = "FilterMethod";
static constexpr const char ThresholdingMethodKey[] = "ThresholdingMethod";
static constexpr const char EdgeDetectionMethodKey[] = "EdgeDetectionMethod";
static constexpr const char CircleFitMethodKey[] = "CircleFitMethod";
static constexpr const char ErrorCorrectionMethodKey[] = "ErrorCorrectionMethod";
static constexpr const char FilterRadiusKey[] = "FilterRadius";
static constexpr const char GaussianSigmaKey[] = "GaussianSigma";
static constexpr const char ColorRadiusKey[] = "ColorRadius";
static constexpr const char MaxLevelKey[] = "MaxLevel";
static constexpr const char PTileValueKey[] = "PTileValue";

static constexpr const char SettingFile[] = "/config.ini";

void saveConfigs(const Configuration& config, QString group)
{
    QSettings settings(qApp->applicationDirPath() + SettingFile, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue(FilterMethodKey,valueToKey(config.filterMethod()));
    settings.setValue(ThresholdingMethodKey,valueToKey(config.thresholdingMethod()));
    settings.setValue(EdgeDetectionMethodKey,valueToKey(config.edgeDetectionMethod()));
    settings.setValue(CircleFitMethodKey,valueToKey(config.circleFitMethod()));
    settings.setValue(ErrorCorrectionMethodKey,valueToKey(config.errorCorrectionMethod()));
    settings.setValue(FilterRadiusKey,config.filterRadius());
    settings.setValue(GaussianSigmaKey,config.gaussianSigma());
    settings.setValue(ColorRadiusKey,config.colorRadius());
    settings.setValue(MaxLevelKey,config.maxLevel());
    settings.setValue(PTileValueKey,config.pTileValue());
    settings.endGroup();
}

Configuration loadConfigs(QString group)
{
    QSettings settings(qApp->applicationDirPath() + SettingFile, QSettings::IniFormat);
    Configuration config;
    settings.beginGroup(group);
    config.setFilterMethod(keyToValue(settings.value(FilterMethodKey).toString(),DefaultFilterMethod))
          .setThresholdingMethod(keyToValue(settings.value(ThresholdingMethodKey).toString(),DefaultThresholdingMethod))
          .setEdgeDetectionMethod(keyToValue(settings.value(EdgeDetectionMethodKey).toString(),DefaultEdgeDetectionMethod))
          .setCircleFitMethod(keyToValue(settings.value(CircleFitMethodKey).toString(),DefaultCircleFitMethod))
          .setErrorCorrectionMethod(keyToValue(settings.value(ErrorCorrectionMethodKey).toString(),DefaultErrorCorrectionMethod))
          .setFilterRadius(settings.value(FilterRadiusKey,DefaultFilterRadius).toUInt())
          .setGaussianSigma(settings.value(GaussianSigmaKey,DefaultGaussianSigma).toReal())
          .setColorRadius(settings.value(ColorRadiusKey,DefaultColorRadius).toReal())
          .setMaxLevel(settings.value(MaxLevelKey,DefaultMaxLevel).toUInt())
          .setPTileValue(settings.value(PTileValueKey,DefaultPTileValue).toReal());
    settings.endGroup();
    return config;
}
