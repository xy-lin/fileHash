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

    QString _fullFilePath;   

protected:

private:
};


std::unordered_map<std::string, ImageFileInfo> imageStore;

long total;

void listFiles(QDir directoryIn)
{    
    QStringList filters;
    filters << "*.JPG" << "*.jpg" << "*.JPEG" << "*.jpeg" << "*.png" << "*.PNG" << "*.bmp" << "*.BMP";

    directoryIn.setNameFilters(filters);
    directoryIn.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

    QFileInfoList list = directoryIn.entryInfoList();

    foreach(QFileInfo finfo, list)
    {
        if ( finfo.isFile() && finfo.size() >= 2048)
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

                ++total;
            }
        }
        else if ( finfo.isDir())
        {         
            listFiles(QDir(finfo.absoluteFilePath()));
        }
    }  
}

QString getOverwriteFileName(QString path, QString inName)
{
    QString newName = path + QString("/") + inName;

    std::string test1 = inName.toStdString();

    std::string  test2 = path.toStdString();

    std::string  test3 = newName.toStdString();
    int i = 0;

    while (QFile::exists(newName))
    {
        newName = path + QString("/") + std::to_string(i).c_str() + "_" + inName;

        std::string test = newName.toStdString();

        ++i;
    }

    return newName;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDir dirIn(QObject::tr(argv[1]));
    QDir dirOut(QObject::tr(argv[2]));

    total = 0;
    listFiles(dirIn);

    std::cout << "total files processed: " << total << std::endl;

    std::cout << "unique files: " << imageStore.size() << std::endl;

    for(std::pair<std::string, ImageFileInfo> kv : imageStore)
    {
        QString dstName = getOverwriteFileName(dirOut.absolutePath(), kv.second._fileName);

        QFile::copy(kv.second._fullFilePath, dstName);  
    }      

    return 1;
}
