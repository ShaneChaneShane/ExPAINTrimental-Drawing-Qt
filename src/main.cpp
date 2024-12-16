#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QFileDialog>
#include <QDebug>
#include <QStyleFactory>



int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    w.setWindowTitle("ExPAINTrimental Drawing Qt");
    QIcon icon(":/resources/PAINTt.png");
    w.setWindowIcon(icon);
    w.show();


    return app.exec();
}

