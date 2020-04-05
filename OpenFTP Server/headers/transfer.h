#pragma once
#include "stdafx.h"
#include "worker_manager.h"

class Transfer
{

public:
	Transfer(QUuid _userGuid,QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir, QString _directoryToReturn);
	Transfer(QUuid _userGuid, QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir);
	Transfer();

	quint64 writeUpload(const QByteArray& data);
	bool finishUpload();
	void cancelUpload();

	QUuid userGuid;
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