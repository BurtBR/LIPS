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
    void On_buttonInsertAnchor_clicked();
    void On_buttonRemoveAnchor_clicked();
    void On_buttonSaveAnchor_clicked();
    void On_buttonSaveAsAnchor_clicked();
    void On_buttonOpenAnchor_clicked();
    void On_lineFx_EditingFinished();
    void On_lineFy_EditingFinished();
    void On_lineCx_EditingFinished();
    void On_lineCy_EditingFinished();
    void On_lineK1_EditingFinished();
    void On_lineK2_EditingFinished();
    void On_lineP1_EditingFinished();
    void On_lineP2_EditingFinished();
    void On_lineR11_EditingFinished();
    void On_lineR12_EditingFinished();
    void On_lineR13_EditingFinished();
    void On_lineR21_EditingFinished();
    void On_lineR22_EditingFinished();
    void On_lineR23_EditingFinished();
    void On_lineR31_EditingFinished();
    void On_lineR32_EditingFinished();
    void On_lineR33_EditingFinished();

signals:
    void VideoPlayerInit();
    void VideoSetFileName(QString);
    void VideoPlay();
    void VideoPause();
    void VideoStop();
    void SetVideoGrayscale(bool);
    void SetFx(double);
    void SetFy(double);
    void SetCx(double);
    void SetCy(double);
    void SetK1(double);
    void SetK2(double);
    void SetP1(double);
    void SetP2(double);
};
#endif // MAINWINDOW_H
