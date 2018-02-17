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

    enum ErrorEliminateMethod
    {
        NoEliminate,
    };
    Q_ENUM(ErrorEliminateMethod)

    FilterMethod filterMethod() const;
    ThresholdingMethod thresholdingMethod() const;
    EdgeDetectionMethod edgeDetectionMethod() const;
    CircleFitMethod circleFitMethod() const;
    ErrorEliminateMethod errorEliminateMethod() const;
    uint filterRadius() const;
    qreal gaussianSigma() const;
    qreal pTileValue() const;

    Configuration& setFilterMethod(FilterMethod method);
    Configuration& setThresholdingMethod(ThresholdingMethod method);
    Configuration& setEdgeDetectionMethod(EdgeDetectionMethod method);
    Configuration& setCircleFitMethod(CircleFitMethod method);
    Configuration& setErrorEliminateMethod(ErrorEliminateMethod method);
    Configuration& setFilterRadius(uint radius);
    Configuration& setGaussianSigma(qreal sigma);
    Configuration& setPTileValue(qreal value);

    static FilterMethod defaultFilterMethod();
    static ThresholdingMethod defaultThresholdingMethod();
    static EdgeDetectionMethod defaultEdgeDetectionMethod();
    static CircleFitMethod defaultCircleFitMethod();
    static ErrorEliminateMethod defaultErrorEliminateMethod();
    static uint defaultFilterRadius();
    static qreal defaultGaussianSigma();
    static qreal defaultPTileValue();

    friend bool operator!=(const Configuration& lhs, const Configuration& rhs);
    friend bool operator==(const Configuration& lhs, const Configuration& rhs);
    friend QDebug operator<<(QDebug dbg, const Configuration& config);

private:
    QSharedDataPointer<ConfigurationData> data;
};

void saveConfigs(const Configuration& config, QString group);
Configuration loadConfigs(QString group);

#endif // CONFIGURATION_H
