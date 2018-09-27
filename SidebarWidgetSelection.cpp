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

#include "CustomizedItemModel.h"
#include <QFile>

void 
SidebarWidgetSelection::addInputWidget(const QString &name, SimCenterWidget *theWidget){
    QStandardItem *item = new QStandardItem(name);
    rootNode->appendRow(item);
    widgets[name] = theWidget;
    widgetIndices[name] = numWidgets;
    numWidgets++;
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
    treeView->setMaximumWidth(200);

    // set up so that a slection change triggers the selectionChanged slot
    QItemSelectionModel *selectionModel= treeView->selectionModel();
    connect(selectionModel,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

    // add the TreeView widget to the layout
    horizontalLayout->addWidget(treeView);
    horizontalLayout->addStretch();
}

SidebarWidgetSelection::SidebarWidgetSelection(QWidget *parent) 
    : SimCenterWidget(parent), currentWidget(0)
{
    horizontalLayout = new QHBoxLayout();
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

    QFile file(":/styles/stylesheet.qss");
    if(file.open(QFile::ReadOnly)) {
        this->setStyleSheet(file.readAll());
        file.close();
        qDebug() << "Open Style File Successfully.";
    }
    else
        qDebug() << "Open Style File Failed!";

}

SidebarWidgetSelection::~SidebarWidgetSelection()
{

}


void
SidebarWidgetSelection::setSelection(const QString &newSelection)
{
    // remove current widget from layout
    if (currentWidget != 0) {
        horizontalLayout->removeWidget(currentWidget);
        currentWidget->setParent(0);
    }

    // find replacement given inut & add that one to layout

    currentWidget = widgets[newSelection];
    int widgetIndex= widgetIndices[newSelection];
    if (currentWidget == 0)
        qDebug() << "WIDGET NOT FOUND";
    else {
        horizontalLayout->insertWidget(horizontalLayout->count()-1, currentWidget, 1);
       // standardModel->index(0,0);
        treeView->setCurrentIndex(treeView->model()->index(widgetIndex,0));
    }

}


void 	
SidebarWidgetSelection::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{
    // remove current widget from layout
    if (currentWidget != 0) {
        horizontalLayout->removeWidget(currentWidget);
        currentWidget->setParent(0);
    }

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    // get widget based on text supplied & that one to layout
    currentWidget = widgets[selectedText];
    if (currentWidget == 0)
        qDebug() << "WIDGET NOT FOUND";
    else
        horizontalLayout->insertWidget(horizontalLayout->count()-1, currentWidget, 1);
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
