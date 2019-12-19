#pragma once
#include "stdafx.h"
#include "serialization_manager.h"
#include "file.h"

class FileManager
{

public:
	static QFileIconProvider::IconType getIconTypeFromString(const QString& type);
	static QString FileManager::getPreviousFolderPath(QString fullpath);
	static bool checkFileExists(const QString& filePath, const QString& fileName);
	static QString changeFileName(const QString& fileName, const QString& filePath);
	static QList<File> getFileListFromJson(const QJsonArray& jsonArray); //*** Remove from this class
	static bool checkIfSensitiveDirectory(const QString& pathDir); //*** Remove from this class

private:
	FileManager();

};

