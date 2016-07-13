#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QProcess>
#include <QTextStream>

#include <cstdlib>
#include <iostream>
#include <unordered_map>

class ImageFileInfo
{
public:
    ImageFileInfo(QString fileName, QString directoryName, QString  fullPath, QString year  )
        :_fileName(fileName), _directoryName(directoryName), _fullFilePath(fullPath), _year(year)
    {
    }

    QString _fileName;
    QString _directoryName;
    QString _fullFilePath;   
    QString _year; 
protected:

private:
};


std::unordered_map<std::string, ImageFileInfo> imageStore;

long total;

QString getCreatedYear( QString inName )
{
    QString metaFileName("C:/meta.txt");
    QString exi("C:/Users/xilin/development/projects/fileHash/thirdParty/exiv2-0.25-win/exiv2.exe pr ");    
    exi.append("\"").append(inName).append("\"").append(" > ").append(metaFileName);    
    
    std::system(exi.toStdString().c_str());

    QString year;
    QFile metaFile(metaFileName);

    if ( metaFile.open(QIODevice::ReadOnly)) 
    {
        QString searchString("Image timestamp");
        QTextStream inStream(&metaFile);
        QString line;     

        do 
        {
            line = inStream.readLine();

            if (line.contains(searchString, Qt::CaseSensitive))             
            {
                 year = line.mid(18, 4);
                 break;
            }
        } 
        while (!line.isNull());       
    }

    metaFile.close();

    return year;
}

void listFiles(QDir directoryIn)
{    
    QStringList filtersPic;
    filtersPic << "*.JPG" << "*.jpg" << "*.JPEG" << "*.jpeg" << "*.png" << "*.PNG" << "*.bmp" << "*.BMP";
  
    directoryIn.setNameFilters(filtersPic);
    directoryIn.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList list = directoryIn.entryInfoList();

    QString dirName = directoryIn.dirName();

    foreach(QFileInfo fileInfo, list)
    {
        if ( fileInfo.isFile() && fileInfo.size() >= 2048)
        {
            QString fileName = fileInfo.fileName();
            QString fullPath = directoryIn.absolutePath();

            QString inName = fullPath + QString("/") + fileName;          
            QFile file(inName);

            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray fileData = file.readAll();
                QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5); 

                QString year;// = getCreatedYear(inName);

                imageStore.insert(std::make_pair(hashData.toHex().toStdString(), ImageFileInfo(fileName, dirName, inName, year )));     

                ++total;
            }
        }
        else if ( fileInfo.isDir())
        {         
            listFiles(QDir(fileInfo.absoluteFilePath()));
        }
    }  
}

QString getOverwriteFileName(QString path, QString year, QString dirName, QString inName)
{
    QString pathName = path /*+ QString("/") + year*/ + QString("/") + dirName + QString("/"); 
    if (!QDir(pathName).exists())    
        QDir().mkdir(pathName);
    
    QString newName = pathName + inName;
    
    int i = 0;
    while (QFile::exists(newName))
    {
        newName = pathName + std::to_string(i).c_str() + "_" + inName;
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

	char* pc = new char[256];
	std::cin >> pc;

    for(std::pair<std::string, ImageFileInfo> kv : imageStore)
    {
        std::cout << kv.second._fileName.toStdString() << std::endl;

        QString dstName = getOverwriteFileName(dirOut.absolutePath(), kv.second._year, kv.second._directoryName, kv.second._fileName);

        QFile::copy(kv.second._fullFilePath, dstName);  
    }      

    return 1;
}
