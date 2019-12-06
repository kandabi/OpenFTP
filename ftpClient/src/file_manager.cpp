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



