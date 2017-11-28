#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QTreeView>
#include <QStandardItemModel>

class SidebarWidgetSelection;
class SimCenterWidget;
class InputWidgetFEM;
class InputWidgetUQ;
class RandomVariableInputWidget;
class DakotaResultsSampling;


class MainWindow : public QMainWindow
{
  Q_OBJECT
    
    public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  public slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

    void onRunButtonClicked();

  //void selectionChangedSlot(const QItemSelection &, const QItemSelection &);

 private:
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    void createActions();

    //Ui::MainWindow *ui;

    QString currentFile;
    SidebarWidgetSelection *inputWidget;

//    SimCenterWidget *edp;
    InputWidgetFEM *fem;
    InputWidgetUQ *uq;
    RandomVariableInputWidget *random;
    DakotaResultsSampling *results;
};

#endif // MAINWINDOW_H
