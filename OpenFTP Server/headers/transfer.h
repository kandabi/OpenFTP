#pragma once
#include "stdafx.h"
#include "worker_manager.h"

class Transfer
{

public:
	Transfer(int _userIndex,QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir, QString _directoryToReturn);
	Transfer(int _userIndex, QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir);
	Transfer();

	quint64 writeUpload(const QByteArray& data);
	bool finishUpload();
	void cancelUpload();

	int userIndex = -1;
	quint64 writtenBytes = 0;
	QString fileName;
	QString filePath;
	bool isBaseDir;
	quint64 fileSize = -1;
	QString directoryToReturn;
	int numOfPacketsSent = 0;
	
	WorkerThread* workerThread = Q_NULLPTR;

private:
	QSaveFile* saveFile = Q_NULLPTR;
};