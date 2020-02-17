#pragma once
#include "stdafx.h"
#include "user.h"

class Transfer
{

public:
	Transfer(int _userIndex,QString _fileName, QString _filePath, int _fileSize, bool _isBaseDir, QString _directoryToReturn);
	Transfer(int _userIndex, QString _fileName, QString _filePath, bool _isBaseDir);
	Transfer();

	int writeUpload(const QByteArray& data);
	bool finishUpload();
	void cancelUpload();

	int userIndex = -1;
	int writtenBytes = 0;
	QString fileName;
	QString filePath;
	bool isBaseDir;
	int fileSize = -1;
	QString directoryToReturn;
	int numOfPacketsSent = 0;

private:
	QSaveFile* file = Q_NULLPTR;

};