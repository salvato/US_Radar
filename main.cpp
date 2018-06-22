#include "mainwindow.h"
#include <QApplication>

int
main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(800, 600);

    QPalette pal = w.palette();
    pal.setColor(QPalette::Window, QRgb(0x40434a));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    w.setPalette(pal);

    w.resize(800, 600);
    w.grabGesture(Qt::PanGesture);
    w.grabGesture(Qt::PinchGesture);
    w.show();
    return a.exec();
}
