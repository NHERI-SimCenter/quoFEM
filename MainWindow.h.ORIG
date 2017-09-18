#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class RandomVariableInputWidget;
class SamplingMethodInputWidget;
class InputWidgetUQ;
class InputWidgetEDP;
class InputWidgetFEM;

class QVBoxLayout;

namespace Ui {
class MainWindow;
}

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

private:
    void createActions();

    // methods to create the widgets
    void makeHeader(void);
    void makeFooter(void);
    void makeRV(void);
    void makeEDP(void);
    void makeUQMethod(void);
    void makeFEM(void);

    // the widgets the above methods create
    RandomVariableInputWidget *rvWidget;
    InputWidgetEDP *edpWidget;
    InputWidgetFEM *femWidget;
    SamplingMethodInputWidget  *uqWidget;

    // methods for saving/loading from files
    void setCurrentFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    // file for saving to and loading from
    QString currentFile;

    // main layout
    QVBoxLayout *layout;


};

#endif // MAINWINDOW_H
