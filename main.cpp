#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>

#include <iostream>
#include <unordered_map>

class ImageFileInfo
{
public:
    ImageFileInfo(QString  fileName, QString  fullPath)
        :_fileName(fileName), _fullFilePath(fullPath)
    {
    }

    QString _fileName;

    QString  _fullFilePath;   

protected:

private:
};


std::unordered_map<std::string, ImageFileInfo> imageStore;

void listFiles(QDir directoryIn, QDir directoryOut)
{    
    QStringList filters;
    filters << "*.JPG" << "*.jpg" << "*.JPEG" << "*.jpeg" << "*.png" << "*.PNG" << "*.bmp" << "*.BMP";

    directoryIn.setNameFilters(filters);
    directoryIn.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

    QFileInfoList list = directoryIn.entryInfoList();

    foreach(QFileInfo finfo, list)
    {
        if ( finfo.isFile())
        {
           QString fileName = finfo.fileName();
            QString  fullPath = directoryIn.absolutePath();

            QString inName = fullPath + QString("/") + fileName;          
            QFile file(inName);

            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray fileData = file.readAll();

                QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5); 
                
                imageStore.insert(std::make_pair(hashData.toHex().toStdString(), ImageFileInfo(fileName, inName)));                 
            }
        }
        else if ( finfo.isDir())
        {         
            listFiles(QDir(finfo.absoluteFilePath()), directoryOut);
        }
    }  
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QDir dirIn(QObject::tr(argv[1]));
    QDir dirOut(QObject::tr(argv[2]));

    listFiles(dirIn, dirOut);
  
    for(std::pair<std::string, ImageFileInfo> kv : imageStore)
    {
        QString dstName = dirOut.absolutePath() + QString("/") + kv.second._fileName;    
        
        QFile::copy(kv.second._fullFilePath, dstName);  
    }      

    return 1;
}

