#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "workervideo.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);
}

MainWindow::~MainWindow(){
    DeleteThread(&_threadVideo);
    delete _ui;
}

void MainWindow::DeleteThread(QThread **threadptr){
    QThread *ptr = *threadptr;

    if(ptr){
        ptr->quit();
        if(!ptr->wait(5000)){
            ptr->terminate();
            ptr->wait();
        }
        delete *threadptr;
        *threadptr = nullptr;
    }
}

bool MainWindow::Init(){

    connect(_ui->buttonOpenFile, &QToolButton::clicked, this, &MainWindow::On_buttonOpenFile_clicked);
    connect(_ui->buttonPlay, &QToolButton::clicked, this, &MainWindow::On_buttonPlay_clicked);
    connect(_ui->buttonStop, &QToolButton::clicked, this, &MainWindow::On_buttonStop_clicked);

    if(!StartThreadVideo())
        return false;

    emit VideoPlayerInit();

    return true;
}

bool MainWindow::StartThreadVideo(){

    if(_threadVideo)
        return false;

    WorkerVideo *worker= nullptr;

    try{
        _threadVideo = new QThread();
    }catch(...){
        _threadVideo = nullptr;
        return false;
    }

    try{
        worker = new WorkerVideo();
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        return false;
    }

    connect(_threadVideo, &QThread::finished, worker, &WorkerVideo::deleteLater);
    connect(worker, &WorkerVideo::ErrorMessage, this, &MainWindow::ConsoleMessage);
    connect(worker, &WorkerVideo::EndOfMedia, this, &MainWindow::VideoEnded);
    connect(worker, &WorkerVideo::FrameReady, this, &MainWindow::FrameReady);
    connect(this, &MainWindow::VideoPlayerInit, worker, &WorkerVideo::Init);
    connect(this, &MainWindow::VideoSetFileName, worker, &WorkerVideo::SetFilename);
    connect(this, &MainWindow::VideoPlay, worker, &WorkerVideo::Play);
    connect(this, &MainWindow::VideoPause, worker, &WorkerVideo::Pause);
    connect(this, &MainWindow::VideoStop, worker, &WorkerVideo::Stop);

    worker->moveToThread(_threadVideo);
    _threadVideo->start();

    return true;
}

void MainWindow::ConsoleMessage(QString text){
    _ui->textConsole->append(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + text);
}

void MainWindow::FrameReady(QPixmap frame){
    int w = _ui->labelImage->width()-10;
    int h = _ui->labelImage->height()-10;

    _ui->labelImage->setPixmap(frame.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::VideoEnded(){
    _isPlaying = false;
}

void MainWindow::On_buttonOpenFile_clicked(){
    QString filename = QFileDialog::getOpenFileName(this, "Open Video", "", "Video (*.mp4 *.wav)");
    if(filename.size())
        emit VideoSetFileName(filename);
}

void MainWindow::On_buttonPlay_clicked(){
    _isPlaying ^= 1;

    if(_isPlaying)
        emit VideoPlay();
    else
        emit VideoStop();
}

void MainWindow::On_buttonStop_clicked(){
    emit VideoStop();
}
