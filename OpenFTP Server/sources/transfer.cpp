#include "stdafx.h"
#include "transfer.h"

Transfer::Transfer(QUuid _userGuid,QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir, QString _directoryToReturn) :
   userGuid{ _userGuid } ,fileName {_fileName }, filePath{ _filePath }, fileSize{ _fileSize }, isBaseDir{ _isBaseDir }, directoryToReturn{ _directoryToReturn }
{
	saveFile = new QSaveFile(filePath + "/" + fileName);
	bool open = saveFile->open(QIODevice::WriteOnly);
};

Transfer::Transfer(QUuid _userGuid, QString _fileName, QString _filePath, quint64 _fileSize, bool _isBaseDir) :
	userGuid{ _userGuid }, fileName{ _fileName }, filePath{ _filePath }, fileSize{ _fileSize }, isBaseDir{ _isBaseDir } {};

Transfer::Transfer() {};

quint64 Transfer::writeUpload(const QByteArray& data)
{
	Q_ASSERT(saveFile != Q_NULLPTR);
	writtenBytes += saveFile->write(data);
	return writtenBytes;
}

void Transfer::cancelUpload()
{
	if (saveFile != Q_NULLPTR && saveFile->isOpen())
	{
		saveFile->cancelWriting();
		saveFile->deleteLater();
	}
}

bool Transfer::finishUpload()
{
	bool commited;
	if (saveFile != Q_NULLPTR && saveFile->isOpen())
	{
		commited = saveFile->commit();
		saveFile->deleteLater();
	}
	
	return commited;
}

