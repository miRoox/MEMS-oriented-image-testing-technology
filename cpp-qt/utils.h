#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>
#include <cmath>
#include <limits>
#include <algorithm>
#include <chrono>
#include <QtDebug>
#include <QThread>

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

#endif // UTILS_H
