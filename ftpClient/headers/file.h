#pragma once
#include "stdafx.h"

class File
{

public:
	File(QString _fileName, QString _filePath, int _fileSize, bool _isDir, QString _lastModified ,QPixmap _icon = {})
		: fileName{ _fileName }, filePath{ _filePath }, fileSize{ _fileSize }, isDir{ _isDir }, lastModified{ _lastModified }, icon{ _icon } {};

	File() {};

	QString fileName;
	QString filePath;
	int fileSize;
	bool isDir;
	QString lastModified;
	QPixmap icon;
};