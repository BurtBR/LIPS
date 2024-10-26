#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMatrix3x3>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

private:
    Ui::MainWindow *_ui;
    QThread *_threadVideo = nullptr, *_threadImageProcessing = nullptr, *_threadFileHandling = nullptr, *_threadPositioning = nullptr;
    bool _isPlaying = false;
    int _frameBalance = 0;

    void DeleteThread(QThread **threadptr);
    bool StartThreadVideo();
    bool StartThreadFileHandling();
    bool StartThreadPositioning();

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
    void SetDefaultValues(uint8_t saturation, uint32_t scalewidth, uint32_t anchormin, uint32_t anchormax, float clock);
    void SetFxFromFile(QString value);
    void SetFyFromFile(QString value);
    void SetCxFromFile(QString value);
    void SetCyFromFile(QString value);
    void SetK1FromFile(QString value);
    void SetK2FromFile(QString value);
    void SetP1FromFile(QString value);
    void SetP2FromFile(QString value);
    void SetRmatrix(QMatrix3x3 rmatrix);
    void AppendAnchorFromFile(QString code, float X, float Y, float Z);
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
    void SetFx(float);
    void SetFy(float);
    void SetCx(float);
    void SetCy(float);
    void SetK1(float);
    void SetK2(float);
    void SetP1(float);
    void SetP2(float);
    void FileLoadDefault();
    void AnchorFileLoad(QString);
};
#endif // MAINWINDOW_H
