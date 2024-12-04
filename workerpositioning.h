#ifndef WORKERPOSITIONING_H
#define WORKERPOSITIONING_H

#include <QObject>
#include <QTableWidget>
#include <QMap>
#include <QMatrix3x3>
#include <QPair>

class WorkerPositioning : public QObject{
    Q_OBJECT
public:
    enum class PositioningModel{
        Trilateration,
        Proportions,
        Pinhole
    };

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
    float _avgframespersymbol = 0, _fps = 1, _clockfreq = 1, _s = 1, _AOV = 65;
    QVector<Anchor> _anchors;
    QMatrix3x3 _K, _R;
    QMap<QString,Anchor> _validAnchors;
    PositioningModel _currModel = PositioningModel::Trilateration;
    QPair<qsizetype,qsizetype> _imagesize = {1280,768};

    void CheckCodes();
    void ResetFound();
    void ResetFrames();
    void InsertFound(QRect impos);
    bool Validate(QString &code, uint8_t &tableidx);
    uint8_t Ones(QString code);
    void Positioning();
    float CalculateHeight();
    float PDistance(const QPair<float,float> &p1, const QPair<float,float> &p2);

public:
    WorkerPositioning(QObject *parent = nullptr);
    static QString GetModelStr(PositioningModel modeltype);

public slots:
    void SetCurrentModel(PositioningModel model);
    void SetAnchorSource(QTableWidget *w);
    void ResetResults();
    void AnchorsInFrame(QVector<QRect> found);
    void SetImageSize(qsizetype width, qsizetype height);
    void SetFPS(float fps);
    void SetClockFreq(float clock);
    void SetFx(float value);
    void SetFy(float value);
    void SetCx(float value);
    void SetCy(float value);
    void SetS(float value);
    void SetR(QMatrix3x3 R);

signals:
    void Message(QString);
};

#endif // WORKERPOSITIONING_H
