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
    uint32_t _minradius=1, _maxradius=20;
    bool _saturation = false, _filter = false, _distortion = false, _positioning = false;

    void ThresholdImage(QImage &frame);
    void FilterImage(QImage &frame);
    QVector<QRect> GroupSaturation(QImage &frame);
    void RecursiveGroup(QImage &frame, uint32_t x, uint32_t y, QRect &anchor);

public:
    WorkerImageProcessing(QObject *parent = nullptr);

public slots:
    void ProcessFrame(QImage frame);
    void SetFPS(float fps);
    void SetThreshold(int value);
    void SetMinRadius(int value);
    void SetMaxRadius(int value);
    void SetSaturationOn(bool condition);
    void SetFilterOn(bool condition);
    void SetDistortionOn(bool condition);
    void SetPositionOn(bool condition);

signals:
    void FrameReady(QPixmap frame);
};

#endif // WORKERIMAGEPROCESSING_H
