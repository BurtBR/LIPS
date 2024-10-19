#ifndef WORKERIMAGEPROCESSING_H
#define WORKERIMAGEPROCESSING_H

#include <QObject>
#include <QPixmap>

//TEMP
#include <QDebug>
//TEMP

class WorkerImageProcessing : public QObject{
    Q_OBJECT
private:
    float _fps = 0;
    uint8_t _threshold = 230;

    void ThresholdImage(QImage &frame);
    void FilterImage(QImage &frame);
    int MultIfValidPixel(QImage &frame, int x, int y, int &value);

public:
    WorkerImageProcessing(QObject *parent = nullptr);

public slots:
    void ProcessFrame(QImage frame);
    void SetFPS(float fps);
    void SetSaturation(int value);

signals:
    void FrameReady(QPixmap frame);
};

#endif // WORKERIMAGEPROCESSING_H
