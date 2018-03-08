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

void ProgressUpdaterContext::end()
{
    Q_CHECK_PTR(ptr);
    ptr->setTextTip("");
    ptr->end();
}
