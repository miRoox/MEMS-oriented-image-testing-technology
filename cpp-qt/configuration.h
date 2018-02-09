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
    Configuration& operator=(const Configuration& );
    ~Configuration();

private:
    QSharedDataPointer<ConfigurationData> data;
};

#endif // CONFIGURATION_H
