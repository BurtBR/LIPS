#ifndef WORKERFILEHANDLER_H
#define WORKERFILEHANDLER_H

#include <QObject>
#include <QFile>

class WorkerFileHandler : public QObject{
    Q_OBJECT
private:
    static volatile uint8_t* _defaultfile_map;
    static QFile *_defaultfile;
    const char *_defaultfilename = "defaulvalues.lips";
    QString _defaultanchorfile;

    bool CreateDefaultFile();
public:
    WorkerFileHandler(QObject *parent = nullptr);
    ~WorkerFileHandler();

public slots:
    void GetDefaultValues();
    void SetSaturation(uint8_t value);
    void SetScaleWidth(uint32_t value);
    void SetLaserMax(uint32_t value);
    void SetLaserMin(uint32_t value);
    void SetClock(double value);

signals:
    void Message(QString);
    void SetDefaultValues(uint8_t, uint32_t, uint32_t, uint32_t, float);
};

#endif // WORKERFILEHANDLER_H
