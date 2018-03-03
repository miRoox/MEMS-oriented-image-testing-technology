#ifndef SNAPSHOTVIEW_H
#define SNAPSHOTVIEW_H

#include <QWidget>
#include <QPixmap>

class QLabel;

class SnapshotView : public QWidget
{
    Q_OBJECT
public:
    explicit SnapshotView(QWidget *parent = nullptr);

    QPixmap pixmap() const;
    QPixmap snapshot() const;

signals:

public slots:
    void setPixmap(const QPixmap& pixmap);
    void setImage(const QImage& image);

protected:
    virtual void enterEvent(QEvent* e) override;
    virtual void leaveEvent(QEvent* e) override;

private slots:
    void saveImage();

private:
    QLabel* view;
    QPixmap image;
};

#endif // SNAPSHOTVIEW_H
