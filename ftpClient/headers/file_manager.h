#pragma once
#include "stdafx.h"


class FileManager
{

public:
	static QFileIconProvider::IconType getIconTypeFromString(const QString& type);
	static QString FileManager::getPreviousFolderPath(QString fullpath);

private:
	FileManager();

};

