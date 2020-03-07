#pragma once
#include "stdafx.h"

class File
{

public:
	File(QString _fileName, QString _filePath, quint64 _fileSize, bool _isDir, QString _lastModified ,QPixmap _icon = {})
		: fileName{ _fileName }, filePath{ _filePath }, fileSize{ _fileSize }, isDir{ _isDir }, lastModified{ _lastModified }, icon{ _icon } {};

	File() {};

	bool isEmpty()
	{
		return fileName.isEmpty();
	}


	QString fileName;
	QString filePath;
	QString lastModified;
	quint64 fileSize = 0;
	bool isDir = false;
	QPixmap icon;
};