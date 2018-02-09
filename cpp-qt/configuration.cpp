#include "configuration.h"
#include <QSharedData>

class ConfigurationData : public QSharedData
{
public:

};

Configuration::Configuration()
    : data(new ConfigurationData)
{

}

Configuration::Configuration(const Configuration& rhs)
    : data(rhs.data)
{

}

Configuration& Configuration::operator=(const Configuration& rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Configuration::~Configuration()
{

}
