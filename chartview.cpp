#include "chartview.h"
#include <QMouseEvent>
#include <QValueAxis>


ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
{
}


ChartView::ChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
}


