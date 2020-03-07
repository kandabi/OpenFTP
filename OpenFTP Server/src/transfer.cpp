#include "stdafx.h"
#include "transfer.h"

Transfer::Transfer(int _userIndex,QString _fileName, QString _filePath, qint64 _fileSize, bool _isBaseDir, QString _directoryToReturn) :
   userIndex{ _userIndex } ,fileName {_fileName }, filePath{ _filePath }, fileSize{ _fileSize }, isBaseDir{ _isBaseDir }, directoryToReturn{ _directoryToReturn }
{
	file = new QSaveFile(filePath + "/" + fileName);
	bool open = file->open(QIODevice::WriteOnly);
};

Transfer::Transfer(int _userIndex, QString _filePath, QString _fileName, bool _isBaseDir) :
	userIndex{ _userIndex }, filePath{ _filePath }, fileName{ _fileName }, isBaseDir{ _isBaseDir } {};

Transfer::Transfer() {};

int Transfer::writeUpload(const QByteArray& data)
{
	Q_ASSERT(file != Q_NULLPTR);
	int writtenData = file->write(data);
	writtenBytes += writtenData;
	return writtenBytes;
}

void Transfer::cancelUpload()
{
	if (file != Q_NULLPTR && file->isOpen())
	{
		file->cancelWriting();
		file->deleteLater();
	}
}

bool Transfer::finishUpload()
{
	bool commited;
	if (file != Q_NULLPTR && file->isOpen())
	{
		commited = file->commit();
		file->deleteLater();
	}
	
	return commited;
}

