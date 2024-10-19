#ifndef WORKERVIDEO_H
#define WORKERVIDEO_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QPixmap>

class WorkerVideo : public QObject{
    Q_OBJECT

private:
    QMediaPlayer *_player = nullptr;
    qint64  _mediasize = 0;
    uint32_t _scalewidth = 500;
    bool _first = false;

public:
    WorkerVideo(QObject *parent = nullptr);
    ~WorkerVideo();

private slots:
    void ProcessFrame(QVideoFrame frame);
    void MediaStatusChanged(QMediaPlayer::MediaStatus status);
    void MediaMetadataChanged();
    void MediaProgressChanged(qint64 ms);

public slots:
    void Init();
    void SetFilename(QString filename);
    void SetScale(int scale);
    void Play();
    void Stop();
    void Pause();

signals:
    void FrameReady(QImage);
    void ErrorMessage(QString);
    void PlayerPlay();
    void PlayerStop();
    void PlayerPause();
    void EndOfMedia();
    void PlayerSetSource(QUrl);
    void ProgressChanged(int);
    void VideoFPSChanged(float);
    void FrameSent();
};

#endif // WORKERVIDEO_H
