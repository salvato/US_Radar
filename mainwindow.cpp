#include "mainwindow.h"
#include "chartview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    radar.setTheme(QChart::ChartThemeBlueCerulean);
    radar.setAnimationOptions(QChart::SeriesAnimations);
    radar.legend()->hide();

    ChartView* pChartView;
    pChartView = new ChartView(&radar);
    pChartView->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(pChartView);
}


MainWindow::~MainWindow()
{
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    radar.stopArduino();
}

