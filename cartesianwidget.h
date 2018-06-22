#ifndef CARTESIANWIDGET_H
#define CARTESIANWIDGET_H

#include <QWidget>

// From the manuals:
// If you intend to use Qt Charts C++ classes in your application,
// use the following include and using directives:
#include <QtCharts>
using namespace QtCharts;


class CartesianWidget : public QChart
{
    Q_OBJECT
public:
    explicit CartesianWidget(QGraphicsItem *parent = nullptr);

signals:

private:
};

#endif // CARTESIANWIDGET_H
