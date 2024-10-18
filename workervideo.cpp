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
    connect(sink, &QVideoSink::videoFrameChanged, this, &WorkerVideo::ProcessFrame);
}

void WorkerVideo::SetFilename(QString filename){

    if(!QFileInfo::exists(filename)){
        emit ErrorMessage("<font color=\"Red\">WorkerVideo: File doesn't exists");
        return;
    }

    emit PlayerSetSource(QUrl(filename));
    emit ErrorMessage("WorkerVideo: File loaded " + filename);
}

void WorkerVideo::ProcessFrame(QVideoFrame frame){
    if(!frame.isValid())
        return;
    QImage frameimage = frame.toImage();
    emit FrameReady(QPixmap::fromImage(frameimage));
}

void WorkerVideo::MediaStatusChanged(QMediaPlayer::MediaStatus status){
    if(status == QMediaPlayer::EndOfMedia){
        emit EndOfMedia();
    }
}

void WorkerVideo::MediaMetadataChanged(){
    _currentFPS = _player->metaData().value(QMediaMetaData::VideoFrameRate).toFloat();
    emit ErrorMessage("WorkerVideo: " + QString::number(_currentFPS) + " FPS");
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
