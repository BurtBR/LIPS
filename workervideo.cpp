#include "workervideo.h"
#include <QFileInfo>
#include <QVideoSink>
#include <QMediaMetaData>

WorkerVideo::WorkerVideo(QObject *parent) : QObject{parent}{

}

WorkerVideo::~WorkerVideo(){
    if(_player){
        delete _player;
        _player = nullptr;
    }
}

void WorkerVideo::Init(){
    if(_player)
        return;

    QVideoSink *sink;

    try{
        _player = new QMediaPlayer;
    }catch(...){
        emit ErrorMessage("<font color=\"Red\">WorkerVideo: Couldn't start player");
        return;
    }

    try{
        sink = new QVideoSink;
    }catch(...){
        delete _player;
        _player = nullptr;
        emit ErrorMessage("<font color=\"Red\">WorkerVideo: Couldn't start player video sink");
        return;
    }

    _player->setVideoSink(sink);

    connect(this, &WorkerVideo::PlayerPlay, _player, &QMediaPlayer::play);
    connect(this, &WorkerVideo::PlayerPause, _player, &QMediaPlayer::pause);
    connect(this, &WorkerVideo::PlayerStop, _player, &QMediaPlayer::stop);
    connect(this, &WorkerVideo::PlayerSetSource, _player, &QMediaPlayer::setSource);
    connect(_player, &QMediaPlayer::destroyed, sink, &QVideoSink::deleteLater);
    connect(_player, &QMediaPlayer::mediaStatusChanged, this, &WorkerVideo::MediaStatusChanged);
    connect(_player, &QMediaPlayer::metaDataChanged, this, &WorkerVideo::MediaMetadataChanged);
    connect(_player, &QMediaPlayer::positionChanged, this, &WorkerVideo::MediaProgressChanged);
    connect(sink, &QVideoSink::videoFrameChanged, this, &WorkerVideo::ProcessFrame);
}

void WorkerVideo::SetFilename(QString filename){

    if(!QFileInfo::exists(filename)){
        emit ErrorMessage("<font color=\"Red\">WorkerVideo: File doesn't exists");
        return;
    }

    emit EndOfMedia();
    emit PlayerSetSource(QUrl(filename));
    emit ErrorMessage("WorkerVideo: File loaded " + filename);
    _first = true;
    emit PlayerPlay();
}

void WorkerVideo::SetScale(int scale){
    if(scale > 10)
        _scalewidth = scale;
}

void WorkerVideo::SetGrayscale(bool value){
    _gray = value;
}

void WorkerVideo::ProcessFrame(QVideoFrame frame){
    if(!frame.isValid())
        return;

    if(_first){
        _first = false;
        emit PlayerStop();
    }

    if(_gray)
        emit FrameReady(frame.toImage().scaled(_scalewidth,_scalewidth,Qt::KeepAspectRatio).convertToFormat(QImage::Format_Grayscale8));
    else
        emit FrameReady(frame.toImage().scaled(_scalewidth,_scalewidth,Qt::KeepAspectRatio));
    emit FrameSent();
}

void WorkerVideo::MediaStatusChanged(QMediaPlayer::MediaStatus status){
    if(status == QMediaPlayer::EndOfMedia){
        emit EndOfMedia();
    }
}

void WorkerVideo::MediaMetadataChanged(){
    float fps = _player->metaData().value(QMediaMetaData::VideoFrameRate).toFloat();
    _mediasize = _player->duration();
    emit ErrorMessage("WorkerVideo: " + QString::number(fps) + " FPS");
    emit VideoFPSChanged(fps);
}

void WorkerVideo::MediaProgressChanged(qint64 ms){
    emit ProgressChanged((ms*10000)/_mediasize);
}

void WorkerVideo::Play(){
    if(!_player){
        emit ErrorMessage("WorkerVideo: Player not initialized");
        emit EndOfMedia();
    }

    if(!_player->hasVideo()){
        emit ErrorMessage("WorkerVideo: No media to play");
        emit EndOfMedia();
    }

    emit PlayerPlay();
}

void WorkerVideo::Stop(){
    emit PlayerStop();
}

void WorkerVideo::Pause(){
    emit PlayerPause();
}
