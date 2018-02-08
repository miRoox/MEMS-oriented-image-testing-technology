#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>

class Processor : public QObject
{
    Q_OBJECT
public:
    explicit Processor(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PROCESSOR_H