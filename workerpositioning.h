#ifndef WORKERPOSITIONING_H
#define WORKERPOSITIONING_H

#include <QObject>

class WorkerPositioning : public QObject{
    Q_OBJECT
public:
    struct Anchor{
        QString code;
        float X,Y,Z;
    };

private:


public:
    WorkerPositioning(QObject *parent = nullptr);

signals:
};

#endif // WORKERPOSITIONING_H
