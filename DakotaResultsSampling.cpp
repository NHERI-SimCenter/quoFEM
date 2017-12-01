// Written: fmckenna

#include "DakotaResultsSampling.h"
#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
using namespace QtCharts;


DakotaResultsSampling::DakotaResultsSampling(QWidget *parent)
    : SimCenterWidget(parent)
{
    // title & add button
    QHBoxLayout *titleLayout = new QHBoxLayout();
    tabWidget = new QTabWidget(this);
    titleLayout->addWidget(tabWidget,1);
    this->setLayout(titleLayout);
    mLeft = true;
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

extern QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit);

int DakotaResultsSampling::processResults(QString &filenameResults, QString &filenameTab) {

    //
    // get a Qwidget ready to place the EDP names, mean, stdDev into
    //

    QWidget *summary = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summary->setLayout(summaryLayout);

    //
    // into a QTextEdit will place contents of Dakota out file
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents

    dakotaText->setText("\n");

    // open file
qDebug() << filenameResults;

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

    // now copy line and every subsequent line into text editor
    dakotaText->append(haystack.c_str());

    bool isSummaryDone = false;

     while (std::getline(fileResults, haystack)) {
        dakotaText->append(haystack.c_str());
        if (isSummaryDone == false) {

            if ( strlen(haystack.c_str()) == 0) {
                    isSummaryDone = true;
            } else {
            std::istringstream iss(haystack);
            std::string subs;

            QWidget *edp = new QWidget;
            QHBoxLayout *edpLayout = new QHBoxLayout();

            edp->setLayout(edpLayout);
            QLineEdit *nameLineEdit;
            QWidget *name = addLabeledLineEdit(QString("Name"), &nameLineEdit);
            iss >> subs;
            nameLineEdit->setText(QString::fromStdString(subs));
            edpLayout->addWidget(name);

            QLineEdit *meanLineEdit;
            QWidget *mean = addLabeledLineEdit(QString("Mean"), &meanLineEdit);
            iss >> subs;
            meanLineEdit->setText(QString::fromStdString(subs));
            meanLineEdit->setMaximumWidth(200);
            edpLayout->addWidget(mean);

            QLineEdit *stdDevLineEdit;
            QWidget *stdDev = addLabeledLineEdit(QString("StdDev"), &stdDevLineEdit);
            iss >> subs;
            stdDevLineEdit->setText(QString::fromStdString(subs));
            stdDevLineEdit->setMaximumWidth(200);
            edpLayout->addWidget(stdDev);

            edpLayout->addStretch();

            summaryLayout->addWidget(edp);
            }
        }
     }
     summaryLayout->addStretch();

    fileResults.close();

    //
    // now into a QTableWidget copy for each simulation the random variable and edp's
    //
    spreadsheet = new MyTableWidget();

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
    theHeadings << "Rum #";
    do
       {
        std::string subs;
        iss >> subs;
        if (colCount > 1) {
            if (subs != " ") {
              theHeadings << subs.c_str();
            }
        }
           colCount++;
       } while (iss);

    colCount = colCount-2;
    spreadsheet->setColumnCount(colCount);
    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    // now until end of file, read lines and set data in QTAbleWidget

    int rowCount = 0;
    while (std::getline(tabResults, inputLine)) {
       std::istringstream is(inputLine);
       int col=0;
       spreadsheet->insertRow(rowCount);
       for (int i=0; i<colCount+2; i++) {
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
  rowCount;
  spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));


  //
  // create a chart, setting data points from first and last col of spreadsheet
  //

  chart = new QChart();
  chart->setAnimationOptions(QChart::AllAnimations);
  QScatterSeries *series = new QScatterSeries;
  col1 = 0;
  col2 = colCount-1;

  for (int i=0; i<rowCount; i++) {
      QTableWidgetItem *itemX = spreadsheet->item(i,col1);
      QTableWidgetItem *itemY = spreadsheet->item(i,col2);
      itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
      itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
      series->append(itemX->text().toDouble(), itemY->text().toDouble());
  }

 // series->setName("Line 1");
 // QVXYModelMapper *mapper = new QVXYModelMapper(this);
 // mapper->setXColumn(0);
 // mapper->setYColumn(colCount);
 // mapper->setSeries(series);
 // mapper->setModel(spreadsheet);

  chart->addSeries(series);
  chart->createDefaultAxes();

  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->chart()->legend()->hide();

  QWidget *widget = new QWidget();
  QVBoxLayout *layout = new QVBoxLayout(widget);
  layout->addWidget(chartView, 1);
  layout->addWidget(spreadsheet, 1);

  tabWidget->addTab(summary,tr("Summmary"));
  tabWidget->addTab(dakotaText, tr("General"));
  tabWidget->addTab(widget, tr("Data Values"));


   tabWidget->adjustSize();

    return 0;
}

void
DakotaResultsSampling::onSpreadsheetCellClicked(int row, int col)
{
    mLeft = spreadsheet->wasLeftKeyPressed();

    // create a new series
    chart->removeAllSeries();

    QScatterSeries *series = new QScatterSeries;

    int oldCol;
    if (mLeft == true) {
        oldCol= col2;
        col2 = col;
    } else {
        oldCol= col1;
        col1 = col;
    }

    int rowCount = spreadsheet->rowCount();
    //QLineSeries *series = new QLineSeries;
    for (int i=0; i<rowCount; i++) {
        QTableWidgetItem *itemX = spreadsheet->item(i,col1);
        QTableWidgetItem *itemY = spreadsheet->item(i,col2);
        QTableWidgetItem *itemOld = spreadsheet->item(i,oldCol);
        itemOld->setData(Qt::BackgroundRole, QColor(Qt::white));
        itemX->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
        itemY->setData(Qt::BackgroundRole, QColor(Qt::lightGray));

        series->append(itemX->text().toDouble(), itemY->text().toDouble());
    }

    chart->addSeries(series);
    chart->createDefaultAxes();
}
