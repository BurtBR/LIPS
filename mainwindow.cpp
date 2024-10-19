#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDateTime>
#include "workervideo.h"
#include "workerimageprocessing.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);
}

MainWindow::~MainWindow(){
    DeleteThread(&_threadVideo);
    DeleteThread(&_threadImageProcessing);
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
    WorkerImageProcessing *workerImage = nullptr;

    try{
        _threadVideo = new QThread;
    }catch(...){
        _threadVideo = nullptr;
        return false;
    }

    try{
        _threadImageProcessing = new QThread;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        _threadImageProcessing = nullptr;
        return false;
    }

    try{
        worker = new WorkerVideo;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        delete _threadImageProcessing;
        _threadImageProcessing = nullptr;
        return false;
    }

    try{
        workerImage = new WorkerImageProcessing;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        delete _threadImageProcessing;
        _threadImageProcessing = nullptr;
        delete worker;
        worker = nullptr;
        return false;
    }

    connect(_threadImageProcessing, &QThread::finished, workerImage, &WorkerImageProcessing::deleteLater);
    connect(_threadVideo, &QThread::finished, worker, &WorkerVideo::deleteLater);

    connect(worker, &WorkerVideo::ErrorMessage, this, &MainWindow::ConsoleMessage);
    connect(worker, &WorkerVideo::EndOfMedia, this, &MainWindow::VideoEnded);
    connect(worker, &WorkerVideo::ProgressChanged, this, &MainWindow::VideoProgressChanged);

    connect(workerImage, &WorkerImageProcessing::FrameReady, this, &MainWindow::FrameReady);

    connect(worker, &WorkerVideo::FrameReady, workerImage, &WorkerImageProcessing::ProcessFrame);
    connect(worker, &WorkerVideo::VideoFPSChanged, workerImage, &WorkerImageProcessing::SetFPS);

    connect(this, &MainWindow::VideoPlayerInit, worker, &WorkerVideo::Init);
    connect(this, &MainWindow::VideoSetFileName, worker, &WorkerVideo::SetFilename);
    connect(this, &MainWindow::VideoPlay, worker, &WorkerVideo::Play);
    connect(this, &MainWindow::VideoPause, worker, &WorkerVideo::Pause);
    connect(this, &MainWindow::VideoStop, worker, &WorkerVideo::Stop);

    worker->moveToThread(_threadVideo);
    workerImage->moveToThread(_threadImageProcessing);
    _threadImageProcessing->start();
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

void MainWindow::VideoProgressChanged(int filled){
    _ui->progressBar->setValue(filled);
}

void MainWindow::VideoEnded(){
    _isPlaying = false;
    _ui->buttonPlay->setIcon(QIcon(":/Figures/Play.png"));
}

void MainWindow::On_buttonOpenFile_clicked(){
    QString filename = QFileDialog::getOpenFileName(this, "Open Video", "", "Video (*.mp4 *.wav)");
    if(filename.size())
        emit VideoSetFileName(filename);
}

void MainWindow::On_buttonPlay_clicked(){
    _isPlaying ^= 1;

    if(_isPlaying){
        _ui->buttonPlay->setIcon(QIcon(":/Figures/Pause.png"));
        emit VideoPlay();
    }else{
        _ui->buttonPlay->setIcon(QIcon(":/Figures/Play.png"));
        emit VideoPause();
    }
}

void MainWindow::On_buttonStop_clicked(){
    emit VideoStop();
    _ui->buttonPlay->setIcon(QIcon(":/Figures/Play.png"));
    _isPlaying = false;
}
