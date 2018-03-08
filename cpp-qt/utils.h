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


#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>
#include <cmath>
#include <limits>
#include <algorithm>
#include <chrono>
#include <QtDebug>
#include <QThread>
#include "progressupdater.h"

#define MAYBE_INTERRUPT_X(ret)  \
    if (QThread::currentThread()->isInterruptionRequested()) do { \
        qCritical() << __func__ << "has been interrupted."; \
        return ret; \
    } while(false)

#define MAYBE_INTERRUPT() MAYBE_INTERRUPT_X( {} )

#define TIMING(expr) [&]()->decltype(expr){ \
    auto start = ::std::chrono::steady_clock::now(); \
    auto&& result = expr; \
    auto end = ::std::chrono::steady_clock::now(); \
    ::std::chrono::duration<double> diff = end-start; \
    qInfo() << "Timing: Evaluating" << #expr << "took" << diff.count() << "s"; \
    return result; }()

#ifdef NO_TIMING_OUTPUT
#undef TIMING
#define TIMING(expr) expr
#endif

#define PROGRESS_UPDATE(percentage) \
    ProgressUpdater::instance()->increaseToValue(100.*percentage)

#endif // UTILS_H
