#include "./headers/stdafx.h"
#include "./headers/file_manager.h"


QFileIconProvider::IconType FileManager::getIconTypeFromString(const QString& type)
{
	if (type == "File Folder")
	{
		return QFileIconProvider::Folder;
	}
	else
		return QFileIconProvider::File;
}


QString FileManager::getPreviousFolderPath(QString fullpath)
{
	if (fullpath.endsWith(":/") || fullpath.endsWith(":"))
		return "";
	else if (fullpath.endsWith("/."))
		fullpath = fullpath.left(fullpath.lastIndexOf('/'));

	return fullpath.left(fullpath.lastIndexOf('/'));

}


bool FileManager::checkFileExists(const QString& filePath, const QString& fileName)
{
	return QDir(filePath).exists(fileName);
}


QString FileManager::changeFileName(const QString& fileName, const QString& filePath)
{
	int fileNumToAppend = 0;
	QString newFileName;
	do
	{
		newFileName = fileName;
		++fileNumToAppend;
		newFileName = newFileName.insert(newFileName.indexOf("."), "_" + QString::number(fileNumToAppend));
	} while (checkFileExists(filePath, newFileName));

	return newFileName;
}
