#include "chartview.h"
#include <QMouseEvent>
#include <QValueAxis>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
}


void
ChartView::wheelEvent(QWheelEvent *event) {
    int degrees = event->delta() / 8;
    QValueAxis* pAxis = (QValueAxis*)chart()->axisY();
    if((degrees > 0) &&  (pAxis->max() > 10.0))
        pAxis->setMax(pAxis->max() - 10.0);
    else if(degrees < 0)
        pAxis->setMax(pAxis->max() + 10.0);
}
