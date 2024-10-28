#ifndef WORKERPOSITIONING_H
#define WORKERPOSITIONING_H

#include <QObject>
#include <QTableWidget>
#include <QMap>

class WorkerPositioning : public QObject{
    Q_OBJECT
public:
    struct Anchor{
        QString code;
        float X,Y,Z;
        QRect im_pos;
        uint32_t frames;
        bool found;
    };

private:
    QTableWidget *_tableSource = nullptr;
    uint8_t _framecounter = 0;
    float _avgframespersymbol = 0, _fps = 1, _clockfreq = 1;
    QVector<Anchor> _anchors;
    QMap<QString,Anchor> _validAnchors;

    void CheckCodes();
    void ResetFound();
    void ResetFrames();
    void InsertFound(QRect impos);
    bool Validate(QString &code, uint8_t &tableidx);
    uint8_t Ones(QString code);

public:
    WorkerPositioning(QObject *parent = nullptr);

public slots:
    void SetAnchorSource(QTableWidget *w);
    void ResetResults();
    void AnchorsInFrame(QVector<QRect> found);
    void SetFPS(float fps);
    void SetClockFreq(float clock);

signals:
    void Message(QString);
};

#endif // WORKERPOSITIONING_H
