#ifndef BAYESPLOTS_H
#define BAYESPLOTS_H

#include <QWidget>
#include <qcustomplot.h>

class BayesPlots : public QWidget
{
    Q_OBJECT
public:
    BayesPlots(QString xLabel, QString yLabel, QWidget *parent = 0);
    ~BayesPlots();

    void plotPosterior(QVector<QVector<double>> minMaxPosterior, QVector<QVector<double>> minMaxPrior,
                       QVector<QVector<double>> statisticsVectorPosterior, QVector<QVector<double>> statisticsVectorPrior,
                       QVector<QVector<double>> statisticsVectorCalData);

private:
    QCustomPlot *thePlot;
};

#endif // BAYESPLOTS_H
