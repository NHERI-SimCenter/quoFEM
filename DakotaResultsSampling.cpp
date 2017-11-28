// Written: fmckenna

#include "DakotaResultsSampling.h"
#include <QTabWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QDebug>
#include <QHBoxLayout>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>

#include <QScreen>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
using namespace QtCharts;
/*
#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qitemmodelbardataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qbar3dseries.h>
#include <QtDataVisualization/q3dtheme.h>

#define USE_STATIC_DATA

using namespace QtDataVisualization;
*/

DakotaResultsSampling::DakotaResultsSampling(QWidget *parent)
    : SimCenterWidget(parent)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();
     tabWidget = new QTabWidget(this);
    titleLayout->addWidget(tabWidget,1);
    this->setLayout(titleLayout);
}

DakotaResultsSampling::~DakotaResultsSampling()
{

}


void DakotaResultsSampling::clear(void)
{

}



void
DakotaResultsSampling::outputToJSON(QJsonObject &jsonObject)
{

}


void
DakotaResultsSampling::inputFromJSON(QJsonObject &jsonObject)
{
  this->clear();

}


int DakotaResultsSampling::processResults(QString &filenameResults, QString &filenameTab) {
  //   tabWidget = new QTabWidget(this);

    /*
    qDebug() << "FILENAME RESULTS :" << filenameResults;
    QFileInfo fileInfo(filenameResults);
    QDir fileDir = fileInfo.absolutePath();
    QString fileName =fileInfo.fileName();
    */

    //
    // into a QTextEdit will place contents of Dakota out file
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents

    dakotaText->setText("Contents Dakota Output File:");

    // open file

    std::ifstream fileResults(filenameResults.toStdString().c_str());
    if (!fileResults.is_open()) {
        qDebug() << "Could not open file: " << filenameResults;
           return -1;
    }

    // now ignore every line until Kurtosis found

    const std::string needle = "Kurtosis";
    std::string haystack;

    while (std::getline(fileResults, haystack)) {
        if (haystack.find(needle) != std::string::npos) {
            break;
        }
    }

    // now copy every subsequent line into text editor
    dakotaText->append(haystack.c_str());

     while (std::getline(fileResults, haystack)) {
        dakotaText->append(haystack.c_str());
     }


    fileResults.close();

    //
    // now into a QTableWidget copy for each simulation the random variable and edp's
    //
    spreadsheet = new QTableWidget();

    // open file
    std::ifstream tabResults(filenameTab.toStdString().c_str());
    if (!tabResults.is_open()) {
        qDebug() << "Could not open file";
           return -1;
    }

    //
    //read first line and set headings (ignoring second column for now)
    //
    std::string inputLine;
    std::getline(tabResults, inputLine);
    std::istringstream iss(inputLine);
    int colCount = 0;
    QStringList theHeadings;
    theHeadings << "Rum";
    do
       {
        std::string subs;
        iss >> subs;
        if (colCount > 1) {
            if (subs != " ") {
      //     std::cerr << " Substring: " << colCount << " " << subs << "\n";
           theHeadings << subs.c_str();
            }
        }
           colCount++;
       } while (iss);


    spreadsheet->setColumnCount(colCount-2);
    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    // now until end of file, read lines and set data in QTAbleWidget

    int rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
       std::istringstream is(inputLine);
       int col=0;
       spreadsheet->insertRow(rowCount);
       for (int i=0; i<colCount; i++) {
           std::string data;
           is >> data;
           if (i != 1) {
               QModelIndex index = spreadsheet->model()->index(rowCount, col);
               spreadsheet->model()->setData(index, data.c_str());
            //qDebug() << rowCount << " " << col << " " << data.c_str();
            col++;
           }
       }
       rowCount++;
    }
  tabResults.close();
  spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);

  /*
  //QChartView *chartView;
  //chartView = new QChartView(createAreaChart());
  //  chartView = new QChartView(createScatterChart());
  QChart *chart = new QChart();
  chart->setTitle("Scatter chart");
 // Q3DBars *graph = new Q3DBars();
 // QWidget *container = QWidget::createWindowContainer(chartView);
  //! [0]

  if (!graph->hasContext()) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't initialize the OpenGL context.");
      msgBox.exec();
      return -1;
  }
  QSize screenSize = graph->screen()->size();
  container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
  container->setMaximumSize(screenSize);
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  container->setFocusPolicy(Qt::StrongFocus);



  //! [1]
  QWidget *widget = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout(widget);
  QTableWidget *tableWidget = new QTableWidget(widget);
  layout->addWidget(chart, 1);
  layout->addWidget(spreadsheet, 1, Qt::AlignHCenter);

*/
    tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(spreadsheet, tr("Data Values"));


     tabWidget->adjustSize();
     fileResults.close();
    return 0;
}
