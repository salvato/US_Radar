#ifndef POLARWIDGET_H
#define POLARWIDGET_H

#include <QWidget>

// From the manuals:
// If you intend to use Qt Charts C++ classes in your application,
// use the following include and using directives:
#include <QtCharts>
using namespace QtCharts;


class PolarWidget : public QPolarChart
{
    Q_OBJECT
public:
    explicit PolarWidget(QGraphicsItem *parent = nullptr);

signals:

private:
};

#endif // POLARWIDGET_H
