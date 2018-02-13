#include "mainwindow.h"
#include <QApplication>
#include "detector.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    detector *detr = new detector(argv[0]);
    MainWindow w(detr);
    w.show();

    return a.exec();
}
