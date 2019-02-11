#ifndef ZIPUTILS_H
#define ZIPUTILS_H
#include <QDir>
#include <QString>

namespace ZipUtils {

bool ZipFolder(QDir directoryToZip, QString zipFilePath);

}
#endif // ZIPUTILS_H
