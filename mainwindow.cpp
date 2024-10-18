#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow){
    _ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete _ui;
}

bool MainWindow::Init(){

    connect(_ui->buttonOpenFile, &QToolButton::clicked, this, &MainWindow::On_buttonOpenFile_clicked);
    return true;
}

void MainWindow::ConsoleMessage(QString text){
    _ui->textConsole->append(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + text);
}

void MainWindow::On_buttonOpenFile_clicked(){
    QFileDialog::getOpenFileName(this, "Open Video", "", "Video (*.mp4 *.wav)");
}
