#ifndef PROGRESSUPDATER_H
#define PROGRESSUPDATER_H

#include <QObject>

class QString;

class ProgressUpdater : public QObject
{
    Q_OBJECT
public:
    explicit ProgressUpdater(QObject *parent = nullptr);
    ~ProgressUpdater();

    static ProgressUpdater* instance(); // singleton

signals:
    void beginRequest();
    void endRequest();
    void valueChange(int value);
    void textTipChange(const QString& text);

public slots:
    void begin();
    void end();
    void increaseToValue(int value);
    void setValue(int value);
    void setTextTip(const QString& text);
private:
    Q_DISABLE_COPY(ProgressUpdater)

    bool state;
    int value;
};


class ProgressUpdaterContext // RAII Container
{
public:
    explicit ProgressUpdaterContext(const QString& textTip);
    ~ProgressUpdaterContext();

    void end();
};

#endif // PROGRESSUPDATER_H
