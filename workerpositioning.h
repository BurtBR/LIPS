#ifndef WORKERPOSITIONING_H
#define WORKERPOSITIONING_H

#include <QObject>
#include <QTableWidget>

class WorkerPositioning : public QObject{
    Q_OBJECT
public:
    struct Anchor{
        QString code;
        float X,Y,Z;
    };

private:
    QTableWidget *_tableSource = nullptr;

public:
    WorkerPositioning(QObject *parent = nullptr);

public slots:
    void SetAnchorSource(QTableWidget *w);
    void UpdateAnchorValues();

signals:
};

#endif // WORKERPOSITIONING_H
