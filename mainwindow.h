#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QFileDialog>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

private:
    Ui::MainWindow *_ui;
    QThread *_threadVideo = nullptr;
    bool _isPlaying = false;

    void DeleteThread(QThread **threadptr);
    bool StartThreadVideo();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool Init();

private slots:
    void ConsoleMessage(QString text);
    void FrameReady(QPixmap frame);
    void VideoEnded();
    void On_buttonOpenFile_clicked();
    void On_buttonPlay_clicked();
    void On_buttonStop_clicked();

signals:
    void VideoPlayerInit();
    void VideoSetFileName(QString);
    void VideoPlay();
    void VideoPause();
    void VideoStop();
};
#endif // MAINWINDOW_H
