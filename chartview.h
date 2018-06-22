#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>

QT_CHARTS_USE_NAMESPACE


class ChartView : public QChartView
{
public:
    ChartView(QWidget *parent = 0);
    ChartView(QChart *chart, QWidget *parent = 0);

private:
};

#endif
