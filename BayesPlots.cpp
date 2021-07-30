#include "bayesplots.h"

BayesPlots::BayesPlots(QString xLabel, QString yLabel, QWidget *parent) : QWidget(parent)
{
//    yMinValue = 0;
//    yMaxValue = 0;
//    xMinValue = 0;
//    xMaxValue = 0;
    // create a main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
//    QTabWidget *tabWidget = new QTabWidget();

    //
    // graphic window
    //

    thePlot=new QCustomPlot();
    thePlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    thePlot->setInteractions(QCP::iSelectPlottables);

    QRect rec = QGuiApplication::screens()[0]->geometry();

    int height = 0.2*rec.height();
    int width = 0.5*rec.width();

    thePlot->setMinimumWidth(width);
    thePlot->setMinimumHeight(height);
//    tabWidget->addWidget(thePlot);
    thePlot->xAxis->setLabel(xLabel);
    thePlot->yAxis->setLabel(yLabel);

//    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(thePlot);

    this->setLayout(mainLayout);
}

BayesPlots::~BayesPlots()
{

}

void BayesPlots::plotPosterior(QVector<QVector<double>> minMaxPosterior, QVector<QVector<double>> minMaxPrior,
                               QVector<QVector<double>> statisticsVectorPosterior, QVector<QVector<double>> statisticsVectorPrior,
                               QVector<QVector<double>> statisticsVectorCalData) {

    int lenData = statisticsVectorCalData.size() - 1;
    int lenPost = statisticsVectorPosterior.size() - 1;

    int start = lenPost - lenData;

    QVector <double> minPosterior(lenData);
    QVector <double> maxPosterior(lenData);
    QVector <double> meanPosterior(lenData);

    QVector <double> minPrior(lenData);
    QVector <double> maxPrior(lenData);
    QVector <double> meanPrior(lenData);

    QVector <double> meanData(lenData);

    QVector <double> x0(lenData);

    for (int i=0; i<lenData; ++i) {
        x0[i] = double(i);
        meanData[i] = statisticsVectorCalData[i+1][0];

        minPosterior[i] = minMaxPosterior[i+1+start][0];
        maxPosterior[i] = minMaxPosterior[i+1+start][1];
        meanPosterior[i] = statisticsVectorPosterior[i+1+start][0];

        minPrior[i] = minMaxPrior[i+1+start][0];
        maxPrior[i] = minMaxPrior[i+1+start][1];
        meanPrior[i] = statisticsVectorPrior[i+1+start][0];
    }

//    thePlot = new QCustomPlot();
    thePlot->legend->setVisible(true);

    QPen pen;

    // add confidence band graphs
    thePlot->addGraph();
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    pen.setColor(QColor(0, 0, 255));
    thePlot->graph(0)->setData(x0, maxPrior);
    thePlot->graph(0)->setName("Range of prior predictions");
    thePlot->graph(0)->setPen(pen);
    thePlot->graph(0)->setBrush(QBrush(QColor(30,50,255,20)));

    thePlot->addGraph();
    thePlot->graph(1)->setPen(pen);
    thePlot->graph(1)->setData(x0, minPrior);
    thePlot->graph(0)->rescaleAxes(true);
    thePlot->graph(1)->rescaleAxes(true);
    thePlot->graph(1)->removeFromLegend();
    thePlot->graph(0)->setChannelFillGraph(thePlot->graph(1));

    thePlot->addGraph();
    pen.setColor(QColor(255, 0, 0));
    thePlot->graph(2)->setPen(pen);
    thePlot->graph(2)->setData(x0, maxPosterior);
    thePlot->graph(2)->setName("Range of posterior predictions");
    thePlot->graph(2)->setBrush(QBrush(QColor(255, 50, 30, 20)));

    thePlot->addGraph();
    thePlot->graph(3)->setPen(pen);
    thePlot->graph(3)->setData(x0, minPosterior);
    thePlot->graph(3)->removeFromLegend();
    thePlot->graph(2)->setChannelFillGraph(thePlot->graph(3));

    // add prediction mean
    thePlot->addGraph();
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    thePlot->graph(4)->setPen(pen);
    thePlot->graph(4)->setData(x0, meanPrior);
    thePlot->graph(4)->setName("Mean of prior predictions");

    thePlot->addGraph();
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    pen.setColor(Qt::red);
    thePlot->graph(5)->setPen(pen);
    thePlot->graph(5)->setData(x0, meanPosterior);
    thePlot->graph(5)->setName("Mean of posterior predictions");

    // add data points graph
    thePlot->addGraph();
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    thePlot->graph(6)->setPen(pen);
    thePlot->graph(6)->setData(x0, meanData);
//    thePlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    thePlot->graph(6)->setName("Mean of data");

    // make top right axes clones of bottom left axes. Looks prettier:
    thePlot->axisRect()->setupFullAxesBox();

//    return thePlot;
}
