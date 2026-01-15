
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "========================================";
    qDebug() << "Phone book application starting";
    qDebug() << "Current directory:" << QDir::currentPath();
    qDebug() << "Executable directory:" << QCoreApplication::applicationDirPath();
    qDebug() << "========================================";
    MainWindow w;
    w.show();
    return a.exec();
}
