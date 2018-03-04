#include "progressupdater.h"
#include <QString>

static ProgressUpdater* ptr = nullptr;

ProgressUpdater::ProgressUpdater(QObject *parent)
    : QObject(parent),state(false),value(-1)
{
    ptr = this;
}

ProgressUpdater::~ProgressUpdater()
{
    ptr = nullptr;
}

ProgressUpdater* ProgressUpdater::instance()
{
    return q_check_ptr(ptr);
}

void ProgressUpdater::begin()
{
    if (state)
        return;
    state = true;
    emit beginRequest();
}

void ProgressUpdater::end()
{
    if (!state)
        return;
    state = false;
    value = 0;
    emit endRequest();
}

void ProgressUpdater::increaseToValue(int value)
{
    if (value > this->value)
        setValue(value);
}

void ProgressUpdater::setValue(int value)
{
    if (this->value == value)
        return;
    emit valueChange(value);
}

void ProgressUpdater::setTextTip(const QString &text)
{
    emit textTipChange(text);
}

ProgressUpdaterContext::ProgressUpdaterContext(const QString &textTip)
{
    Q_CHECK_PTR(ptr);
    ptr->begin();
    ptr->setTextTip(textTip);
}

ProgressUpdaterContext::~ProgressUpdaterContext()
{
    end();
}

void ProgressUpdaterContext::end()
{
    Q_CHECK_PTR(ptr);
    ptr->setTextTip("");
    ptr->end();
}
