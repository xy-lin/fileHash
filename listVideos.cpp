
#include <QFile>

#include <QDebug>
#include <QDir>
#include <QDateTime>

#include <QTextStream>

#include <iomanip>
#include <cstdlib>
#include <iostream>

#include "listVideos.h"



void listVideoFiles(QDir directoryIn, QStringList& allVideosFullPath, QStringList& allVideosFileNames)
{
	long total = 0;

	QStringList filtersPic;
	filtersPic << "*.MOV" << "*.mov" << "*.mp4" << "*.MP4" << "*.avi" << "*.AVI";

	directoryIn.setNameFilters(filtersPic);
	directoryIn.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	QFileInfoList list = directoryIn.entryInfoList();

	QString dirName = directoryIn.dirName();

	

	foreach(QFileInfo fileInfo, list)
	{
		if (fileInfo.isFile() && fileInfo.size() >= 2048)
		{
			QString fileName = fileInfo.fileName();
			QString fullPath = directoryIn.absolutePath();
			QString inName = fullPath + QString("/") + fileName;

			allVideosFullPath.push_back(inName);
			allVideosFileNames.push_back(fileName);
			++total;

			std::cout << "\r" << total << std::flush;
		}
		else if (fileInfo.isDir())
		{
			listVideoFiles(QDir(fileInfo.absoluteFilePath()), allVideosFullPath, allVideosFileNames);
		}
	}	
}
