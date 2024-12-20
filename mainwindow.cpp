#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDateTime>
#include <QDoubleValidator>
#include "workervideo.h"
#include "workerimageprocessing.h"
#include "workerfilehandler.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);
    _ui->tabWidget->setCurrentWidget(_ui->tabPositioning);
}

MainWindow::~MainWindow(){
    DeleteThread(&_threadVideo);
    DeleteThread(&_threadImageProcessing);
    DeleteThread(&_threadFileHandling);
    DeleteThread(&_threadPositioning);
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

    if(!StartMainThreads())
        return false;

    if(!StartThreadFileHandling())
        return false;

    emit SetAnchorSourceTable(_ui->tableAnchors);

    emit VideoPlayerInit();

    try{
        _ui->lineCx->setValidator(new QDoubleValidator(_ui->lineCx));
        _ui->lineCy->setValidator(new QDoubleValidator(_ui->lineCy));
        _ui->lineFx->setValidator(new QDoubleValidator(_ui->lineFx));
        _ui->lineFy->setValidator(new QDoubleValidator(_ui->lineFy));
        _ui->lineK1->setValidator(new QDoubleValidator(_ui->lineK1));
        _ui->lineK2->setValidator(new QDoubleValidator(_ui->lineK2));
        _ui->lineP1->setValidator(new QDoubleValidator(_ui->lineP1));
        _ui->lineP2->setValidator(new QDoubleValidator(_ui->lineP2));

        _ui->lineR11->setValidator(new QDoubleValidator(_ui->lineR11));
        _ui->lineR12->setValidator(new QDoubleValidator(_ui->lineR12));
        _ui->lineR13->setValidator(new QDoubleValidator(_ui->lineR13));
        _ui->lineR21->setValidator(new QDoubleValidator(_ui->lineR21));
        _ui->lineR22->setValidator(new QDoubleValidator(_ui->lineR22));
        _ui->lineR23->setValidator(new QDoubleValidator(_ui->lineR23));
        _ui->lineR31->setValidator(new QDoubleValidator(_ui->lineR31));
        _ui->lineR32->setValidator(new QDoubleValidator(_ui->lineR32));
        _ui->lineR33->setValidator(new QDoubleValidator(_ui->lineR33));
    }catch(...){
        return false;
    }

    connect(_ui->comboPosModel, &QComboBox::currentIndexChanged, this, &MainWindow::On_comboPosModel_currentIndexChanged);
    connect(_ui->buttonOpenFile, &QToolButton::clicked, this, &MainWindow::On_buttonOpenFile_clicked);
    connect(_ui->buttonPlay, &QToolButton::clicked, this, &MainWindow::On_buttonPlay_clicked);
    connect(_ui->buttonStop, &QToolButton::clicked, this, &MainWindow::On_buttonStop_clicked);
    connect(_ui->checkSaturation, &QCheckBox::stateChanged, this, &MainWindow::On_checkSaturation_stateChanged);
    connect(_ui->checkFilter, &QCheckBox::stateChanged, this, &MainWindow::On_checkFilter_stateChanged);
    connect(_ui->checkPosition, &QCheckBox::stateChanged, this, &MainWindow::On_checkPositioning_stateChanged);
    connect(_ui->buttonInsertAnchor, &QToolButton::clicked, this, &MainWindow::On_buttonInsertAnchor_clicked);
    connect(_ui->buttonRemoveAnchor, &QToolButton::clicked, this, &MainWindow::On_buttonRemoveAnchor_clicked);
    connect(_ui->buttonSaveAnchor, &QToolButton::clicked, this, &MainWindow::On_buttonSaveAnchor_clicked);
    connect(_ui->buttonOpenAnchor, &QToolButton::clicked, this, &MainWindow::On_buttonOpenAnchor_clicked);
    connect(_ui->buttonCloarConsole, &QToolButton::clicked, this, &MainWindow::On_buttonClearConsole_clicked);
    connect(_ui->lineFx, &QLineEdit::editingFinished, this, &MainWindow::On_lineFx_EditingFinished);
    connect(_ui->lineFy, &QLineEdit::editingFinished, this, &MainWindow::On_lineFy_EditingFinished);
    connect(_ui->lineCx, &QLineEdit::editingFinished, this, &MainWindow::On_lineCx_EditingFinished);
    connect(_ui->lineCy, &QLineEdit::editingFinished, this, &MainWindow::On_lineCy_EditingFinished);
    connect(_ui->lineK1, &QLineEdit::editingFinished, this, &MainWindow::On_lineK1_EditingFinished);
    connect(_ui->lineK2, &QLineEdit::editingFinished, this, &MainWindow::On_lineK2_EditingFinished);
    connect(_ui->lineP1, &QLineEdit::editingFinished, this, &MainWindow::On_lineP1_EditingFinished);
    connect(_ui->lineP2, &QLineEdit::editingFinished, this, &MainWindow::On_lineP2_EditingFinished);
    connect(_ui->lineS, &QLineEdit::editingFinished, this, &MainWindow::On_lineS_EditingFinished);
    connect(_ui->lineR11, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR12, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR13, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR21, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR22, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR23, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR31, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR32, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);
    connect(_ui->lineR33, &QLineEdit::editingFinished, this, &MainWindow::SendRMatrix);

    _ui->tableAnchors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _ui->tableAnchors->setHorizontalHeaderLabels({"Anchor Code", "Position X", "Position Y", "Position Z"});

    emit FileLoadDefault();

    On_comboPosModel_currentIndexChanged(_ui->comboPosModel->currentIndex());

    return true;
}

void MainWindow::SendRMatrix(){
    QMatrix3x3 R;
    bool ok;

    R.data()[0] = _ui->lineR11->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[1] = _ui->lineR21->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[2] = _ui->lineR31->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[3] = _ui->lineR12->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[4] = _ui->lineR22->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[5] = _ui->lineR32->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[6] = _ui->lineR13->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[7] = _ui->lineR23->text().toFloat(&ok);
    if(!ok)
        return;
    R.data()[8] = _ui->lineR33->text().toFloat(&ok);
    if(!ok)
        return;

    emit SetRMatrix(R);
}

bool MainWindow::StartMainThreads(){

    if(_threadVideo || _threadImageProcessing || _threadPositioning)
        return false;

    WorkerVideo *worker= nullptr;
    WorkerImageProcessing *workerImage = nullptr;
    WorkerPositioning *workerPosition = nullptr;

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
        _threadPositioning = new QThread;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        delete _threadImageProcessing;
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
        delete _threadPositioning;
        _threadPositioning = nullptr;
        return false;
    }

    try{
        workerImage = new WorkerImageProcessing;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        delete _threadImageProcessing;
        _threadImageProcessing = nullptr;
        delete _threadPositioning;
        _threadPositioning = nullptr;
        delete worker;
        worker = nullptr;
        return false;
    }

    try{
        workerPosition = new WorkerPositioning;
    }catch(...){
        delete _threadVideo;
        _threadVideo = nullptr;
        delete _threadImageProcessing;
        _threadImageProcessing = nullptr;
        delete _threadPositioning;
        _threadPositioning = nullptr;
        delete worker;
        worker = nullptr;
        delete workerImage;
        workerImage = nullptr;
        return false;
    }

    connect(_threadImageProcessing, &QThread::finished, workerImage, &WorkerImageProcessing::deleteLater);
    connect(_threadVideo, &QThread::finished, worker, &WorkerVideo::deleteLater);
    connect(_threadPositioning, &QThread::finished, workerPosition, &WorkerPositioning::deleteLater);

    connect(worker, &WorkerVideo::ErrorMessage, this, &MainWindow::ConsoleMessage);
    connect(worker, &WorkerVideo::EndOfMedia, this, &MainWindow::VideoEnded);
    connect(worker, &WorkerVideo::ProgressChanged, this, &MainWindow::VideoProgressChanged);
    connect(worker, &WorkerVideo::FrameSent, this, &MainWindow::VideoSentFrame);

    connect(workerImage, &WorkerImageProcessing::FrameReady, this, &MainWindow::FrameReady);
    connect(workerImage, &WorkerImageProcessing::Message, this, &MainWindow::ConsoleMessage);

    connect(workerPosition, &WorkerPositioning::Message, this, &MainWindow::ConsoleMessage);

    connect(workerImage, &WorkerImageProcessing::AnchorsOnFrame, workerPosition, &WorkerPositioning::AnchorsInFrame);
    connect(workerImage, &WorkerImageProcessing::SetImageSize, workerPosition, &WorkerPositioning::SetImageSize);

    connect(worker, &WorkerVideo::VideoFPSChanged, workerPosition, &WorkerPositioning::SetFPS);

    connect(worker, &WorkerVideo::FrameReady, workerImage, &WorkerImageProcessing::ProcessFrame);

    connect(this, &MainWindow::VideoPlayerInit, worker, &WorkerVideo::Init);
    connect(this, &MainWindow::VideoSetFileName, worker, &WorkerVideo::SetFilename);
    connect(this, &MainWindow::VideoPlay, worker, &WorkerVideo::Play);
    connect(this, &MainWindow::VideoPause, worker, &WorkerVideo::Pause);
    connect(this, &MainWindow::VideoStop, worker, &WorkerVideo::Stop);
    connect(this, &MainWindow::SetVideoGrayscale, worker, &WorkerVideo::SetGrayscale);

    connect(this, &MainWindow::SetFx, workerImage, &WorkerImageProcessing::SetFx);
    connect(this, &MainWindow::SetFy, workerImage, &WorkerImageProcessing::SetFy);
    connect(this, &MainWindow::SetCx, workerImage, &WorkerImageProcessing::SetCx);
    connect(this, &MainWindow::SetCy, workerImage, &WorkerImageProcessing::SetCy);
    connect(this, &MainWindow::SetK1, workerImage, &WorkerImageProcessing::SetK1);
    connect(this, &MainWindow::SetK2, workerImage, &WorkerImageProcessing::SetK2);
    connect(this, &MainWindow::SetP1, workerImage, &WorkerImageProcessing::SetP1);
    connect(this, &MainWindow::SetP2, workerImage, &WorkerImageProcessing::SetP2);

    connect(this, &MainWindow::SetFx, workerPosition, &WorkerPositioning::SetFx);
    connect(this, &MainWindow::SetFy, workerPosition, &WorkerPositioning::SetFy);
    connect(this, &MainWindow::SetCx, workerPosition, &WorkerPositioning::SetCx);
    connect(this, &MainWindow::SetCy, workerPosition, &WorkerPositioning::SetCy);
    connect(this, &MainWindow::SetS, workerPosition, &WorkerPositioning::SetS);
    connect(this, &MainWindow::SetRMatrix, workerPosition, &WorkerPositioning::SetR);
    connect(this, &MainWindow::SetPositioningModel, workerPosition, &WorkerPositioning::SetCurrentModel);
    connect(this, &MainWindow::SetAnchorSourceTable, workerPosition, &WorkerPositioning::SetAnchorSource);
    connect(this, &MainWindow::ResetAnchorResults, workerPosition, &WorkerPositioning::ResetResults);

    connect(_ui->spinClock, &QDoubleSpinBox::valueChanged, workerPosition, &WorkerPositioning::SetClockFreq);

    connect(_ui->spinScale, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetScaleWidth);
    connect(_ui->spinSaturation, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetThreshold);
    connect(_ui->spinMinRadius, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetMinRadius);
    connect(_ui->spinMaxRadius, &QSpinBox::valueChanged, workerImage, &WorkerImageProcessing::SetMaxRadius);
    connect(_ui->checkSaturation, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetSaturationOn);
    connect(_ui->checkFilter, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetFilterOn);
    connect(_ui->checkDistortion, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetDistortionOn);
    connect(_ui->checkPosition, &QCheckBox::stateChanged, workerImage, &WorkerImageProcessing::SetPositionOn);

    worker->moveToThread(_threadVideo);
    workerImage->moveToThread(_threadImageProcessing);
    workerPosition->moveToThread(_threadPositioning);
    _threadPositioning->start();
    _threadImageProcessing->start(QThread::HighestPriority);
    _threadVideo->start();

    return true;
}

bool MainWindow::StartThreadFileHandling(){
    if(_threadFileHandling)
        return false;

    WorkerFileHandler *worker;

    try{
        _threadFileHandling = new QThread;
    }catch(...){
        _threadFileHandling = nullptr;
        return false;
    }

    try{
        worker = new WorkerFileHandler;
    }catch(...){
        delete _threadFileHandling;
        _threadFileHandling = nullptr;
        return false;
    }

    connect(_threadFileHandling, &QThread::finished, worker, &WorkerFileHandler::deleteLater);

    connect(worker, &WorkerFileHandler::Message, this, &MainWindow::ConsoleMessage);
    connect(worker, &WorkerFileHandler::SetDefaultValues, this, &MainWindow::SetDefaultValues);
    connect(worker, &WorkerFileHandler::SetFx, this, &MainWindow::SetFxFromFile);
    connect(worker, &WorkerFileHandler::SetFy, this, &MainWindow::SetFyFromFile);
    connect(worker, &WorkerFileHandler::SetCx, this, &MainWindow::SetCxFromFile);
    connect(worker, &WorkerFileHandler::SetCy, this, &MainWindow::SetCyFromFile);
    connect(worker, &WorkerFileHandler::SetK1, this, &MainWindow::SetK1FromFile);
    connect(worker, &WorkerFileHandler::SetK2, this, &MainWindow::SetK2FromFile);
    connect(worker, &WorkerFileHandler::SetP1, this, &MainWindow::SetP1FromFile);
    connect(worker, &WorkerFileHandler::SetP2, this, &MainWindow::SetP2FromFile);
    connect(worker, &WorkerFileHandler::SetS, this, &MainWindow::SetSFromFile);
    connect(worker, &WorkerFileHandler::SetRmatrix, this, &MainWindow::SetRmatrix);
    connect(worker, &WorkerFileHandler::AppendAnchor, this, &MainWindow::AppendAnchorFromFile);

    connect(this, &MainWindow::FileLoadDefault, worker, &WorkerFileHandler::GetDefaultValues);
    connect(this, &MainWindow::AnchorFileLoad, worker, &WorkerFileHandler::OpenAnchorFile);
    connect(this, &MainWindow::SaveAnchors, worker, &WorkerFileHandler::SaveAnchors);

    connect(_ui->spinSaturation, &QSpinBox::valueChanged, worker, &WorkerFileHandler::SetSaturation);
    connect(_ui->spinScale, &QSpinBox::valueChanged, worker, &WorkerFileHandler::SetScaleWidth);
    connect(_ui->spinMaxRadius, &QSpinBox::valueChanged, worker, &WorkerFileHandler::SetLaserMax);
    connect(_ui->spinMinRadius, &QSpinBox::valueChanged, worker, &WorkerFileHandler::SetLaserMin);
    connect(_ui->spinClock, &QDoubleSpinBox::valueChanged, worker, &WorkerFileHandler::SetClock);

    worker->moveToThread(_threadFileHandling);
    _threadFileHandling->start();

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
    emit ResetAnchorResults();
}

void MainWindow::VideoSentFrame(){
    _frameBalance++;

    if(_ui->checkStopSync->isChecked()){
        if(_frameBalance > 5){
            ConsoleMessage("<font color=\"Red\">MainWindow: " + QString::number(_frameBalance) + " frames behind");
        }

        if(_frameBalance > 15){
            On_buttonStop_clicked();
            ConsoleMessage("<font color=\"Red\">MainWindow: Video interrupted due to inability to maintain sync with the video");
        }else if(_frameBalance > 10){
            ConsoleMessage("<font color=\"Red\">MainWindow: Unable to maintain sync with the video.");
        }
    }
}

void MainWindow::SetDefaultValues(uint8_t saturation, uint32_t scalewidth, uint32_t anchormin, uint32_t anchormax, float clock){

    ConsoleMessage("MainWindow: Default values received");

    _ui->spinSaturation->setValue(saturation);
    _ui->spinScale->setValue(scalewidth);
    _ui->spinMinRadius->setValue(anchormin);
    _ui->spinMaxRadius->setValue(anchormax);
    _ui->spinClock->setValue(clock);
}

void MainWindow::SetFxFromFile(QString value){
    _ui->lineFx->setText(value);
    emit _ui->lineFx->editingFinished();
}

void MainWindow::SetFyFromFile(QString value){
    _ui->lineFy->setText(value);
    emit _ui->lineFy->editingFinished();
}

void MainWindow::SetCxFromFile(QString value){
    _ui->lineCx->setText(value);
    emit _ui->lineCx->editingFinished();
}

void MainWindow::SetCyFromFile(QString value){
    _ui->lineCy->setText(value);
    emit _ui->lineCy->editingFinished();
}

void MainWindow::SetK1FromFile(QString value){
    _ui->lineK1->setText(value);
    emit _ui->lineK1->editingFinished();
}

void MainWindow::SetK2FromFile(QString value){
    _ui->lineK2->setText(value);
    emit _ui->lineK2->editingFinished();
}

void MainWindow::SetP1FromFile(QString value){
    _ui->lineP1->setText(value);
    emit _ui->lineP1->editingFinished();
}

void MainWindow::SetP2FromFile(QString value){
    _ui->lineP2->setText(value);
    emit _ui->lineP2->editingFinished();
}

void MainWindow::SetSFromFile(QString value){
    _ui->lineS->setText(value);
    emit _ui->lineS->editingFinished();
}

void MainWindow::SetRmatrix(QMatrix3x3 rmatrix){
    _ui->lineR11->setText(QString::number(rmatrix(0,0)));
    _ui->lineR12->setText(QString::number(rmatrix(0,1)));
    _ui->lineR13->setText(QString::number(rmatrix(0,2)));
    _ui->lineR21->setText(QString::number(rmatrix(1,0)));
    _ui->lineR22->setText(QString::number(rmatrix(1,1)));
    _ui->lineR23->setText(QString::number(rmatrix(1,2)));
    _ui->lineR31->setText(QString::number(rmatrix(2,0)));
    _ui->lineR32->setText(QString::number(rmatrix(2,1)));
    _ui->lineR33->setText(QString::number(rmatrix(2,2)));
    emit _ui->lineR11->editingFinished();
    emit _ui->lineR12->editingFinished();
    emit _ui->lineR13->editingFinished();
    emit _ui->lineR21->editingFinished();
    emit _ui->lineR22->editingFinished();
    emit _ui->lineR23->editingFinished();
    emit _ui->lineR31->editingFinished();
    emit _ui->lineR32->editingFinished();
    emit _ui->lineR33->editingFinished();
}

void MainWindow::AppendAnchorFromFile(QString code, float X, float Y, float Z){
    int index = _ui->tableAnchors->rowCount();

    _ui->tableAnchors->insertRow(index);
    _ui->tableAnchors->setItem(index, 0, new QTableWidgetItem(code));
    _ui->tableAnchors->setItem(index, 1, new QTableWidgetItem(QString::number(X)));
    _ui->tableAnchors->setItem(index, 2, new QTableWidgetItem(QString::number(Y)));
    _ui->tableAnchors->setItem(index, 3, new QTableWidgetItem(QString::number(Z)));
}

void MainWindow::On_comboPosModel_currentIndexChanged(int idx){
    switch(idx){
    case 0:
        emit SetPositioningModel(WorkerPositioning::PositioningModel::Trilateration);
        break;
    case 1:
        emit SetPositioningModel(WorkerPositioning::PositioningModel::Proportions);
        break;
    case 2:
        emit SetPositioningModel(WorkerPositioning::PositioningModel::Pinhole);
        break;
    default:
        break;
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
    emit ResetAnchorResults();
    _ui->buttonPlay->setIcon(QIcon(":/Figures/Play.png"));
    _isPlaying = false;
}

void MainWindow::On_buttonInsertAnchor_clicked(){
    _ui->tableAnchors->insertRow(0);
    _ui->tableAnchors->setItem(0, 0, new QTableWidgetItem);
    _ui->tableAnchors->setItem(0, 1, new QTableWidgetItem);
    _ui->tableAnchors->setItem(0, 2, new QTableWidgetItem);
    _ui->tableAnchors->setItem(0, 3, new QTableWidgetItem);
}

void MainWindow::On_buttonRemoveAnchor_clicked(){
    while(_ui->tableAnchors->selectedItems().size()){
        _ui->tableAnchors->removeRow(_ui->tableAnchors->selectedItems().at(0)->row());
    }
}

void MainWindow::On_buttonSaveAnchor_clicked(){
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save Anchors", "", "*.csv");

    if(filename.size()){
        QVector<QString> R;
        R.append(_ui->lineR11->text());
        R.append(_ui->lineR12->text());
        R.append(_ui->lineR13->text());
        R.append(_ui->lineR21->text());
        R.append(_ui->lineR22->text());
        R.append(_ui->lineR23->text());
        R.append(_ui->lineR31->text());
        R.append(_ui->lineR32->text());
        R.append(_ui->lineR33->text());
        emit SaveAnchors(filename,
                         _ui->lineFx->text(),
                         _ui->lineFy->text(),
                         _ui->lineCx->text(),
                         _ui->lineCy->text(),
                         _ui->lineK1->text(),
                         _ui->lineK2->text(),
                         _ui->lineP1->text(),
                         _ui->lineP2->text(),
                         _ui->lineS->text(),
                         R,
                         _ui->tableAnchors);
    }
}

void MainWindow::On_buttonOpenAnchor_clicked(){
    QString filename = QFileDialog::getOpenFileName(nullptr, "Open Anchors", "", "*.csv");

    if(filename.size()){
        _ui->tableAnchors->clearContents();
        _ui->tableAnchors->setRowCount(0);
        emit AnchorFileLoad(filename);
    }
}

void MainWindow::On_buttonClearConsole_clicked(){
    _ui->textConsole->clear();
}

void MainWindow::On_lineFx_EditingFinished(){
    emit SetFx(_ui->lineFx->text().toFloat());
}

void MainWindow::On_lineFy_EditingFinished(){
    emit SetFy(_ui->lineFy->text().toFloat());
}

void MainWindow::On_lineCx_EditingFinished(){
    emit SetCx(_ui->lineCx->text().toFloat());
}

void MainWindow::On_lineCy_EditingFinished(){
    emit SetCy(_ui->lineCy->text().toFloat());
}

void MainWindow::On_lineK1_EditingFinished(){
    emit SetK1(_ui->lineK1->text().toFloat());
}

void MainWindow::On_lineK2_EditingFinished(){
    emit SetK2(_ui->lineK2->text().toFloat());
}

void MainWindow::On_lineP1_EditingFinished(){
    emit SetP1(_ui->lineP1->text().toFloat());
}

void MainWindow::On_lineP2_EditingFinished(){
    emit SetP2(_ui->lineP2->text().toFloat());
}

void MainWindow::On_lineS_EditingFinished(){
    emit SetS(_ui->lineS->text().toFloat());
}
