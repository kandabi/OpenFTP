#include "stdafx.h"
#include "ftp_manager.h"


QFileInfoList FtpManager::getFilesFromDirectory(const QString& path, bool isBaseDir)
{
	QDir directory(path);
	if(isBaseDir)
		return directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
	else
		return directory.entryInfoList(QDir::NoDotDot | QDir::AllEntries);
}

QJsonValue FtpManager::encodePixmapForJson(const QPixmap& p) {
	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	bool save = p.save(&buffer, "PNG");
	auto const encoded = buffer.data().toBase64();
	return { QLatin1String(encoded) };
}

QPixmap FtpManager::getIconFromFileInfo(const QFileInfo& file) {
	QFileIconProvider qfileIconProvider;
	QIcon icon = (file.fileName() != ".") ? qfileIconProvider.icon(file) : QIcon("./images/return_button.png");
	return icon.pixmap(icon.actualSize(QSize(16, 16)));
}

QJsonArray FtpManager::createServerResponse(ResponseType responseStatus ,const QString& path, bool isBaseDir, int bytesWritten)
{
	QJsonArray serverResponse;
	
	serverResponse.append(QJsonObject{
			{"directory" ,path} ,
			{"response_status", static_cast<int>(responseStatus)} ,
			{"bytesWritten", bytesWritten} ,
		});

	for (const QFileInfo& file : getFilesFromDirectory(path, isBaseDir))
	{
		QPixmap icon = getIconFromFileInfo(file);

		QJsonObject json
		{
			{"fileName",file.fileName()},
			{"fileSize", file.size()},
			{"filePath", file.absoluteFilePath()},
			{"isDir", file.isDir()},
			{"lastModified", file.lastModified().toString(Qt::DateFormat::SystemLocaleShortDate)},
			{"icon", encodePixmapForJson(icon)},
		};
		serverResponse.append(json);
	}

	return serverResponse;
}


QJsonArray FtpManager::createUploadProgressResponse(ResponseType responseStatus, const QString& path, int bytesWritten)
{
	QJsonArray serverResponse;

	serverResponse.append(QJsonObject{
			{"directory" ,path} ,
			{"response_status", static_cast<int>(responseStatus)} ,
			{"bytesWritten", bytesWritten} ,
		});

	return serverResponse;
}



bool FtpManager::checkIfBaseDir(const QString& directory, const QString& homeDirectory)
{
	return (directory == homeDirectory || directory == "./") ? true : false;
}

bool FtpManager::renameFile(const QString& filePath, const QString& oldFileName,  QString& newFileName)
{
	QDir directory(filePath);
	QFileInfo file(directory, oldFileName);
	if (file.isFile() && !newFileName.contains("."))
	{
		newFileName += "." + file.suffix();
	}
	return directory.rename(oldFileName ,newFileName);
}

bool FtpManager::createFolder(const QString& newFolderPath)
{
	return QDir().mkdir(newFolderPath);
}


bool FtpManager::checkFileExists(const QString& filePath, const QString& fileName)
{
	return QDir(filePath).exists(fileName);
}

Transfer FtpManager::startFileUpload(const int& userIndex ,const QString& fileName, const QString& filePath, const int& fileSize, const bool& baseDir, const QString& directoryToReturn)
{
	return Transfer(userIndex, filePath, fileName, fileSize, baseDir, directoryToReturn);
}


Transfer FtpManager::createPendingFileDownload(const int& userIndex, const QString& filePath, const QString& fileName, const bool& baseDir, QString& errorString)
{
	QFile qfile(filePath + '/' + fileName);

	if (!qfile.open(QIODevice::ReadOnly))
	{
		errorString = "Unable to load file, transfer failed.";
		return Transfer();
	}

	return Transfer(userIndex, filePath, fileName, baseDir);
}


bool FtpManager::beginFileDownload(const Transfer& download, QTcpSocket* socket, QString& errorString)
{
	QFile qfile(download.filePath + '/' + download.fileName);
	if (!qfile.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QByteArray fileData = qfile.readAll();
	if (fileData.isEmpty())
		fileData = " ";
	qint64 result = socket->write(fileData);
	return true;
}


int FtpManager::processFileUpload(const QByteArray& data, Transfer& upload)
{
	upload.writeUpload(data);
	return upload.writtenBytes;
}

void FtpManager::cancelFileUpload(Transfer& upload)
{
	upload.cancelUpload();
}


bool FtpManager::completeFileUpload(Transfer& upload)
{
	return upload.finishUpload();
}


void FtpManager::deleteFiles(const QJsonArray& filesToDelete)
{

	for (int i = 0; i < filesToDelete.count(); ++i)
	{
		 const QString& filePath = filesToDelete[i].toString();

		 QDir directory(filePath);
		 bool remove = directory.remove(filePath);
		 bool result = directory.removeRecursively();

	}
}

QString FtpManager::changeFileName(const QString& fileName, const QString& filePath)
{
	int fileNumToAppend = 0;
	QString newFileName;
	do
	{
		newFileName = fileName;
		++fileNumToAppend;
		newFileName = newFileName.insert(newFileName.indexOf("."), "_" + QString::number(fileNumToAppend));
	} while (checkFileExists(filePath, newFileName));

	return newFileName;
}

