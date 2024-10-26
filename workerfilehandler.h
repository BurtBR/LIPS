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
    uint8_t _saturation;
    uint32_t _scalewidth, _lasermin, _lasermax;
    float _clockfreq;

    bool CreateDefaultFile();
public:
    WorkerFileHandler(QObject *parent = nullptr);
    ~WorkerFileHandler();

public slots:
    void GetDefaultValues();

signals:
    void Message(QString);
    void SetDefaultValues(uint8_t, uint32_t, uint32_t, uint32_t, float);
};

#endif // WORKERFILEHANDLER_H
