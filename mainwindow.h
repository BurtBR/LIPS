#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    QThread *_threadVideo = nullptr, *_threadImageProcessing;
    bool _isPlaying = false;
    int _frameBalance = 0;

    void DeleteThread(QThread **threadptr);
    bool StartThreadVideo();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool Init();

private slots:
    void ConsoleMessage(QString text);
    void FrameReady(QPixmap frame);
    void VideoProgressChanged(int filled);
    void VideoEnded();
    void VideoSentFrame();
    void On_checkSaturation_stateChanged(bool value);
    void On_checkFilter_stateChanged(bool value);
    void On_checkPositioning_stateChanged(bool value);
    void On_buttonOpenFile_clicked();
    void On_buttonPlay_clicked();
    void On_buttonStop_clicked();

signals:
    void VideoPlayerInit();
    void VideoSetFileName(QString);
    void VideoPlay();
    void VideoPause();
    void VideoStop();
    void SetVideoGrayscale(bool);
};
#endif // MAINWINDOW_H
