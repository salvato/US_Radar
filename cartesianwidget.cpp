#include "cartesianwidget.h"


CartesianWidget::CartesianWidget(QGraphicsItem *parent)
    : QChart(parent)
{
    setTitle("Ultrasonic Radar");

    QValueAxis *xAxis = new QValueAxis();
    xAxis->setTickCount(11);
    xAxis->setLabelFormat("%.1f");
    xAxis->setShadesVisible(false);
    xAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    addAxis(xAxis, Qt::AlignBottom);

    QValueAxis *yAxis = new QValueAxis();
    yAxis->setTickCount(11);
    yAxis->setLabelFormat("%.1f");
    yAxis->setShadesVisible(false);
    yAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    addAxis(yAxis, Qt::AlignLeft);

    xAxis->setRange(-300, 300);
    yAxis->setRange(-300, 300);
}

