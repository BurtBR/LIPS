#ifndef WORKERFILEHANDLER_H
#define WORKERFILEHANDLER_H

#include <QObject>
#include <QFile>
#include <QMatrix3x3>
#include <QTableWidget>

class WorkerFileHandler : public QObject{
    Q_OBJECT
private:
    static volatile uint8_t* _defaultfile_map;
    static QFile *_defaultfile;
    const char *_defaultfilename = "defaulvalues.lips";

    bool MapDefaultFile();
    bool UnmapDefaultFile();
    bool CreateDefaultFile();
    bool SetDefaultAnchorFilename(QString filename);
    QString GetDefaultAnchorFilename();
public:
    WorkerFileHandler(QObject *parent = nullptr);
    ~WorkerFileHandler();

public slots:
    void GetDefaultValues();
    void SetSaturation(uint8_t value);
    void SetScaleWidth(uint32_t value);
    void SetLaserMax(uint32_t value);
    void SetLaserMin(uint32_t value);
    void SetClock(float value);
    void SaveAnchors(QString filename, QString Fx, QString Fy, QString Cx, QString Cy, QString K1, QString K2, QString P1, QString P2, QString S, QVector<QString> R, QTableWidget *anchortable);
    void OpenAnchorFile(QString filename);

signals:
    void Message(QString);
    void SetDefaultValues(uint8_t, uint32_t, uint32_t, uint32_t, float);
    void SetFx(QString);
    void SetFy(QString);
    void SetCx(QString);
    void SetCy(QString);
    void SetK1(QString);
    void SetK2(QString);
    void SetP1(QString);
    void SetP2(QString);
    void SetS(QString);
    void SetRmatrix(QMatrix3x3);
    void AppendAnchor(QString,float,float,float);
};

#endif // WORKERFILEHANDLER_H
