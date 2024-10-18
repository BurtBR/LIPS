#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.showMaximized();

    QStyleFactory::create("Windows");

    if(!w.Init()){
        QMessageBox::warning(nullptr, "Error", "Failed to initialize!");
        return 0;
    }

    return a.exec();
}
