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
    float _k1 = -0.379010881029726, _k2 = 0.128105162756552, _p1 = 0, _p2 = 0;
    float _fx = 1030.246379210344, _fy = 1031.711917017123, _cx = 722.1847404744256, _cy = 415.8590977650202;
    uint8_t _threshold = 230;
    uint32_t _minradius=1, _maxradius=20, _scalewidth = 0, _originalwidth, _originalheight;
    bool _saturation = false, _filter = false, _distortion = false, _positioning = false;

    void ThresholdImage(QImage &frame);
    void FilterImage(QImage &frame);
    QVector<QRect> GroupSaturation(QImage &frame);
    void RecursiveGroup(QImage &frame, uint32_t x, uint32_t y, QRect &anchor);
    QRgb MapDistortion(QImage &distorted, float x, float y);

public:
    WorkerImageProcessing(QObject *parent = nullptr);

public slots:
    void ProcessFrame(QImage frame);
    void SetThreshold(int value);
    void SetScaleWidth(int value);
    void SetMinRadius(int value);
    void SetMaxRadius(int value);
    void SetFx(float value);
    void SetFy(float value);
    void SetCx(float value);
    void SetCy(float value);
    void SetK1(float value);
    void SetK2(float value);
    void SetP1(float value);
    void SetP2(float value);
    void SetSaturationOn(bool condition);
    void SetFilterOn(bool condition);
    void SetDistortionOn(bool condition);
    void SetPositionOn(bool condition);

signals:
    void FrameReady(QPixmap frame);
    void Message(QString);
};

#endif // WORKERIMAGEPROCESSING_H
