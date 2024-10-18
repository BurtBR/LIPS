#ifndef WORKERVIDEO_H
#define WORKERVIDEO_H

#include <QObject>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>
#include <QPixmap>

class WorkerVideo : public QObject{
    Q_OBJECT

private:
    QMediaPlayer *_player = nullptr;
    bool _filter = false, _distortion = false, _saturation = false, _position = false;

public:
    WorkerVideo(QObject *parent = nullptr);
    ~WorkerVideo();

private slots:
    void ProcessFrame(QVideoFrame frame);
    void MediaStatusChanged(QMediaPlayer::MediaStatus status);

public slots:
    void Init();
    void SetFilename(QString filename);
    void Play();
    void Stop();
    void Pause();

signals:
    void FrameReady(QPixmap);
    void ErrorMessage(QString);
    void PlayerPlay();
    void PlayerStop();
    void PlayerPause();
    void EndOfMedia();
    void PlayerSetSource(QUrl);
};

#endif // WORKERVIDEO_H
