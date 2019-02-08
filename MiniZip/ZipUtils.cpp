#include <ZipUtils.h>
#include <zip.h>
#include <QDirIterator>

bool ZipUtils::ZipFolder(QDir directoryToZip, QString zipFilePath)
{

    zipFile newZipFile = zipOpen(zipFilePath.toStdString().c_str(), APPEND_STATUS_CREATE);

    //Checking if the file is opened
    if(NULL == newZipFile)
        return false;

    int result = ZIP_OK;

    QDirIterator dirIt(directoryToZip, QDirIterator::Subdirectories);

    //We need to find the parent to get relative path including directory name
    QDir parentDir(directoryToZip.absolutePath());
    parentDir.cdUp();

    //Looping through files and directories
    while (dirIt.hasNext())
    {
        QFileInfo fileInfo(dirIt.next());

        if(fileInfo.isDir())
            continue;

        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        QFile afile(fileInfo.filePath());


        if(afile.exists())
        {

            if(!afile.open(QFile::ReadOnly))
                return false;

            QByteArray fileByteArray = afile.readAll();

            QString relFilePath = parentDir.relativeFilePath(fileInfo.absoluteFilePath());

            result = zipOpenNewFileInZip(newZipFile,
                                         relFilePath.toStdString().c_str(),
                                         NULL,
                                         NULL,
                                         0,
                                         NULL,
                                         0,
                                         NULL,
                                         Z_DEFLATED,
                                         Z_BEST_COMPRESSION);

            //checking if file entry is opened
            if(result != ZIP_OK)
                return false;

            result = zipWriteInFileInZip(newZipFile, fileByteArray.data(), fileByteArray.length());

            //checking if writing was successful
           if(result != ZIP_OK)
                return false;

            //close file inside zip
            result = zipCloseFileInZip(newZipFile);

            //checking if file entry closing was successful
            if(result != ZIP_OK)
                 return false;
        }

    }



    //Closing zip file
    result = zipClose(newZipFile, "Compressed directory");

    return result;
}
