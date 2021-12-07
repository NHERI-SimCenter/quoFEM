/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: bsaakash

#include "BayesPlots.h"

BayesPlots::BayesPlots(QVector<QString> edpNames, QVector<int> edpLengths, QWidget *parent) : QWidget(parent)
{
//    yMinValue = 0;
//    yMaxValue = 0;
//    xMinValue = 0;
//    xMaxValue = 0;
    // create a main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();

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
//    thePlot->xAxis->setLabel(xLabel);
//    thePlot->yAxis->setLabel(yLabel);

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
