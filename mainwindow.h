#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

private:
    Ui::MainWindow *_ui;

    void ConsoleMessage(QString text);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool Init();

private slots:
    void On_buttonOpenFile_clicked();

signals:

};
#endif // MAINWINDOW_H
