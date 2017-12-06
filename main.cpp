#include "MainWindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    //qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QFile file(":/styleCommon/style.qss");
    if(file.open(QFile::ReadOnly)) {
       QString styleSheet = QLatin1String(file.readAll());
       a.setStyleSheet(styleSheet);
    }

w.setStyleSheet("QLineEdit {background: #FFFFFF;}");
w.setStyleSheet("QComboBox {background: #FFFFFF;} QLineEdit {background: #FFFFFF}");

   return a.exec();
}
