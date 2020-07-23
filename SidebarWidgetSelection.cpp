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

// Written: fmckenna

#include "SidebarWidgetSelection.h"
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include <QModelIndex>
#include <QStackedWidget>
#include <QHeaderView>
#include <QVBoxLayout>

#include "CustomizedItemModel.h"
#include <QFile>

void 
SidebarWidgetSelection::addInputWidget(const QString &name, const QString &description, SimCenterWidget *theWidget){
    QStandardItem *item = new QStandardItem(name);
    item->setToolTip(description);
    rootNode->appendRow(item);
    widgets[name] = theWidget;
    widgetIndices[name] = numWidgets;
    numWidgets++;
    theStackedWidget->addWidget(theWidget);
}

void
SidebarWidgetSelection::removeInputWidget(const QString &name){

}


void
SidebarWidgetSelection::buildTreee(){
    //register the model
    treeView->setModel(standardModel);
    treeView->expandAll();
    treeView->setHeaderHidden(true);

    //register the model
    treeView->setModel(standardModel);
    treeView->expandAll();
    treeView->setHeaderHidden(true);
    treeView->setMinimumWidth(100);
    treeView->setMaximumWidth(100);
    treeView->setMinimumWidth(100);

    treeView->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers);//Disable Edit for the TreeView


    /* *************************** EFFORTS TO REMOVE FIXED SIZE WIDGET
    treeView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QHeaderView *header = treeView->header();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    header->setStretchLastSection(false);
    //treeView->resizeColumnToContents(0);
    ***********************************************************************/
   // treeView->setMaximumWidth(200);

    // set up so that a slection change triggers the selectionChanged slot
    QItemSelectionModel *selectionModel= treeView->selectionModel();
    connect(selectionModel,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

    // add the TreeView widget to the layout
    /*
    QVBoxLayout *sideLayout = new QVBoxLayout();
    sideLayout->addWidget(treeView,1);
    QLabel *simLogo = new QLabel();
    QPixmap pixmap2(":/imagesCommon/sim_logo_footer.png");
    QPixmap newPixmap2 = pixmap2;
    int w = pixmap2.width();
    int h = pixmap2.height();
    simLogo->setPixmap(newPixmap2.scaled(w/2,h/2,Qt::KeepAspectRatio));

    sideLayout->addWidget(simLogo);
     horizontalLayout->addLayout(sideLayout,0);
    */


    horizontalLayout->addWidget(treeView,0);


    horizontalLayout->addWidget(theStackedWidget,1);
    horizontalLayout->addStretch();
}

SidebarWidgetSelection::SidebarWidgetSelection(QWidget *parent) 
    : SimCenterWidget(parent), currentWidget(0)
{
    //this->setStyleSheet("background-color:red;");
    this->setContentsMargins(0,5,0,5);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setMargin(0);
    //horizontalLayout->setSpacing(0);
    this->setLayout(horizontalLayout);

    //
    // create a tree widget, assign it a mode and add to layout
    //

    treeView = new QTreeView();
    standardModel = new CustomizedItemModel; // QStandardItemModel ;
    rootNode = standardModel->invisibleRootItem();
    numWidgets = 0;

    //
    // customize the apperance of the menu on the left
    //

    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff ); // hide the horizontal scroll bar
    treeView->setObjectName("treeViewOnTheLeft");
    treeView->setIndentation(0);
    //treeView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    theStackedWidget = new QStackedWidget();
}

SidebarWidgetSelection::~SidebarWidgetSelection()
{

}


void
SidebarWidgetSelection::setSelection(const QString &newSelection)
{
    currentWidget = widgets[newSelection];
    if (currentWidget == 0)
        qDebug() << "WIDGET NOT FOUND";
    else {
        int widgetIndex= widgetIndices[newSelection];
        treeView->setCurrentIndex(treeView->model()->index(widgetIndex,0));
    }
}


void 	
SidebarWidgetSelection::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{
    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();
    int widgetIndex= widgetIndices[selectedText];

    theStackedWidget->setCurrentIndex(widgetIndex);
}

bool
SidebarWidgetSelection::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QMap<QString, SimCenterWidget *>::iterator i;
    for (i = widgets.begin(); i != widgets.end(); ++i)
        if (i.value()->outputToJSON(jsonObject) == false)
            result = false;
    return false;
}

void
SidebarWidgetSelection::clear(void)
{
    //    theClineInput->clear();
}

bool
SidebarWidgetSelection::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QMap<QString, SimCenterWidget *>::iterator i;
    for (i = widgets.begin(); i != widgets.end(); ++i)
        if (i.value()->inputFromJSON(jsonObject) == false)
            result = false;
    return result;
}
