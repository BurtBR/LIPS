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
    double _k1 = -0.379010881029726, _k2 = 0.128105162756552, _p1 = 0, _p2 = 0;
    double _fx = 1030.246379210344, _fy = 1031.711917017123, _cx = 722.1847404744256, _cy = 415.8590977650202;
    uint8_t _threshold = 230;
    uint32_t _minradius=1, _maxradius=20;
    bool _saturation = false, _filter = false, _distortion = false, _positioning = false;

    void ThresholdImage(QImage &frame);
    void FilterImage(QImage &frame);
    QVector<QRect> GroupSaturation(QImage &frame);
    void RecursiveGroup(QImage &frame, uint32_t x, uint32_t y, QRect &anchor);
    QRgb MapDistortion(QImage &distorted, double x, double y);

public:
    WorkerImageProcessing(QObject *parent = nullptr);

public slots:
    void ProcessFrame(QImage frame);
    void SetFPS(float fps);
    void SetThreshold(int value);
    void SetMinRadius(int value);
    void SetMaxRadius(int value);
    void SetFx(double value);
    void SetFy(double value);
    void SetCx(double value);
    void SetCy(double value);
    void SetK1(double value);
    void SetK2(double value);
    void SetP1(double value);
    void SetP2(double value);
    void SetSaturationOn(bool condition);
    void SetFilterOn(bool condition);
    void SetDistortionOn(bool condition);
    void SetPositionOn(bool condition);

signals:
    void FrameReady(QPixmap frame);
};

#endif // WORKERIMAGEPROCESSING_H
