#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <cmath>
#include <limits>
#include <algorithm>
#include <QtDebug>
#include <QThread>

#define MAYBE_INTERRUPT_X(ret)  \
    if (QThread::currentThread()->isInterruptionRequested()) do { \
        qCritical() << __func__ << "has been interrupted."; \
        return ret; \
    } while(false)

#define MAYBE_INTERRUPT() MAYBE_INTERRUPT_X( {} )

#endif // GLOBAL_H
