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



QList<File> FileManager::getFileListFromJson(const QJsonArray& jsonArray)
{
	QList<File> files;
	for (int i = 1; i < jsonArray.count(); ++i)
	{
		QJsonObject json = jsonArray[i].toObject();
		files.append(
			File{
				json.value("fileName").toString(),
				json.value("filePath").toString(),
				json.value("fileSize").toInt(),
				json.value("isDir").toBool(),
				json.value("lastModified").toString(),
				Serializer::decodePixmapFromString(json.value("icon").toString())
			}
		);
	}

	return files;
}

bool FileManager::checkIfSensitiveDirectory(const QString& directory)
{
	QDir pathDir(directory);
	if (pathDir.path().contains(":/Windows"))
	{
		return true;
	}
	else
		return false;

}