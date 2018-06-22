#include "polarwidget.h"


PolarWidget::PolarWidget(QGraphicsItem *parent)
    : QPolarChart(parent)
{
    setTitle("Ultrasonic Radar");

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(false);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(5);
    radialAxis->setMinorTickCount(1);
    radialAxis->setLabelFormat("%d");
    radialAxis->setShadesVisible(false);
    addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    radialAxis->setRange(0.0, 300.0); // Distance in cm
    angularAxis->setRange(0.0, 360.0);// Angles in degrees
}

