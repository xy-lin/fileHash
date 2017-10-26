#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QProcess>
#include <QTextStream>

#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include"listVideos.h"

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

                QString year;

                imageStore.insert(std::make_pair(hashData.toHex().toStdString(), ImageFileInfo(fileName, dirName, inName, year )));

                ++total;

				std::cout << "\r" << total << std::flush;
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

    bool isFile = false;
    if (argc == 3 && argv[2]=="f")
        isFile = true;

    total = 0;
	
    if (!isFile)
	{
        QStringList allVideosFullPath, allVideosFileName;

        listVideoFiles(dirIn, allVideosFullPath, allVideosFileName);

		std::cout << std::endl;
		std::cout << "total files processed: " << allVideosFullPath.size() << std::endl;

		char* pc = new char[256];
		std::cin >> pc;
		int i = 0;
		for (; i < allVideosFullPath.size(); ++i)
		{
			QString fileName = allVideosFullPath.at(i);

			std::cout << "\r" << std::setw(30) << fileName.toStdString() << std::flush;

			QString dstName = getOverwriteFileName(dirOut.absolutePath(), "", "", allVideosFileName.at(i));
			
			QFile::copy(fileName, dstName);
		}
	}
	else
	{
        listFiles(dirIn);

		std::cout << std::endl;
		std::cout << "total files processed: " << total << std::endl;
		std::cout << "unique files: " << imageStore.size() << std::endl;

		char* pc = new char[256];
		std::cin >> pc;

		for (std::pair<std::string, ImageFileInfo> kv : imageStore)
		{
			std::cout << "\r" << std::setw(30) << kv.second._fileName.toStdString() << std::flush;

			QString dstName = getOverwriteFileName(dirOut.absolutePath(), kv.second._year, kv.second._directoryName, kv.second._fileName);

			QFile::copy(kv.second._fullFilePath, dstName);
		}
	}
	

    return 1;
}
