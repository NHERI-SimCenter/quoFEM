#ifndef SIMCENTER_GRAPH_PLOT_H
#define SIMCENTER_GRAPH_PLOT_H

#include <QWidget>
#include <QVector>
#include <QString>

class QCustomPlot;
class QCPGraph;
class QCPCurve;
class QCPItemTracer;

//
// a widget to plot the response,
//  NOTE to cut down on memory currently the widget does not keep data, requires a call back to main window
//  THIS is proving problematic to say the least! .. will redo
//

class SimCenterGraphPlot : public QWidget
{
    Q_OBJECT
public:
    SimCenterGraphPlot(QString xAxis, QString yAxis, QWidget *parent = 0);
    SimCenterGraphPlot(QString xAxis, QString yAxis, int minWidth, int minHeight, QWidget *parent = 0);
    ~SimCenterGraphPlot();

    void clear();
    void addLine(QVector<double> &x, QVector<double> &y, int width = 1, int r=0, int g=0, int b=0);

signals:

public slots:

private:
    QCustomPlot *thePlot;

    QCPGraph *graph;
    QCPItemTracer *groupTracer;
    QCPCurve *curve;
    int numGraphs;
    double xMinValue;
    double xMaxValue;
    double yMinValue;
    double yMaxValue;
};

#endif // NODESIMCENTER_GRAPH_PLOT_H
