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
    connect(_ui->checkSaturation, &QCheckBox::stateChanged, this, &MainWindow::On_checkSaturation_stateChanged);
    connect(_ui->checkFilter, &QCheckBox::stateChanged, this, &MainWindow::On_checkFilter_stateChanged);
    connect(_ui->checkPosition, &QCheckBox::stateChanged, this, &MainWindow::On_checkPositioning_stateChanged);

    if(!StartThreadVideo())
        return false;

    emit VideoPlayerInit();

    _ui->spinScale->setValue(700);
    _ui->spinSaturation->setValue(230);
    _ui->spinMinRadius->setValue(5);
    _ui->spinMaxRadius->setValue(20);

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
    connect(worker, &WorkerVideo::FrameSent, this, &MainWindow::VideoSentFrame);

    connect(workerImage, &WorkerImageProcessing::FrameReady, this, &MainWindow::FrameReady);

    connect(worker, &WorkerVideo::FrameReady, workerImage, &WorkerImageProcessing::ProcessFrame);
    connect(worker, &WorkerVideo::VideoFPSChanged, workerImage, &WorkerImageProcessing::SetFPS);

    connect(this, &MainWindow::VideoPlayerInit, worker, &WorkerVideo::Init);
    connect(this, &MainWindow::VideoSetFileName, worker, &WorkerVideo::SetFilename);
    connect(this, &MainWindow::VideoPlay, worker, &WorkerVideo::Play);
    connect(this, &MainWindow::VideoPause, worker, &WorkerVideo::Pause);
    connect(this, &MainWindow::VideoStop, worker, &WorkerVideo::Stop);
    connect(this, &MainWindow::SetVideoGrayscale, worker, &WorkerVideo::SetGrayscale);

    connect(_ui->spinScale, &QSpinBox::valueChanged, worker, &WorkerVideo::SetScale);
    connect(_ui->spinSaturation, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetThreshold);
    connect(_ui->spinMinRadius, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetMinRadius);
    connect(_ui->spinMaxRadius, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetMaxRadius);
    connect(_ui->checkSaturation, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetSaturationOn);
    connect(_ui->checkFilter, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetFilterOn);
    connect(_ui->checkDistortion, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetDistortionOn);
    connect(_ui->checkPosition, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetPositionOn);

    worker->moveToThread(_threadVideo);
    workerImage->moveToThread(_threadImageProcessing);
    _threadImageProcessing->start(QThread::HighestPriority);
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

    _frameBalance--;
}

void MainWindow::VideoProgressChanged(int filled){
    _ui->progressBar->setValue(filled);
}

void MainWindow::VideoEnded(){
    _isPlaying = false;
    _ui->buttonPlay->setIcon(QIcon(":/Figures/Play.png"));
}

void MainWindow::VideoSentFrame(){
    _frameBalance++;

    if(_frameBalance > 5){
        ConsoleMessage("<font color=\"Red\">MainWindow: " + QString::number(_frameBalance) + " frames behind");
    }

    if(_ui->checkStopSync->isChecked()){
        if(_frameBalance > 15){
            On_buttonStop_clicked();
            ConsoleMessage("<font color=\"Red\">MainWindow: Video interrupted due to inability to maintain sync with the video");
        }else if(_frameBalance > 10){
            ConsoleMessage("<font color=\"Red\">MainWindow: Unable to maintain sync with the video.");
        }
    }
}

void MainWindow::On_checkSaturation_stateChanged(bool value){
    emit SetVideoGrayscale(value);
}

void MainWindow::On_checkFilter_stateChanged(bool value){
    if(value){
        _ui->checkSaturation->setChecked(true);
        _ui->checkSaturation->setDisabled(true);
    }else{
        _ui->checkSaturation->setDisabled(false);
    }
}

void MainWindow::On_checkPositioning_stateChanged(bool value){
    if(value){
        _ui->checkSaturation->setChecked(true);
        _ui->checkFilter->setChecked(true);
        _ui->checkSaturation->setDisabled(true);
        _ui->checkFilter->setDisabled(true);
    }else{
        _ui->checkFilter->setDisabled(false);
    }
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
