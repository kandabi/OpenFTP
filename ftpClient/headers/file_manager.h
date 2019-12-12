#pragma once
#include "stdafx.h"


class FileManager
{

public:
	static QFileIconProvider::IconType getIconTypeFromString(const QString& type);
	static QString FileManager::getPreviousFolderPath(QString fullpath);
	static bool checkFileExists(const QString& filePath, const QString& fileName);
	static QString changeFileName(const QString& fileName, const QString& filePath);

private:
	FileManager();

};

