#pragma once
#include "stdafx.h"
#include "transfer.h"

class FtpManager
{

public:
	enum class RequestType
	{
		Browse,
		Remove,
		Rename,
		CreateFolder,
		UploadFile,
		UploadCompleted,
		DownloadFile,
		DownloadFolder,
		NextPendingDownload,
	};

	enum class ResponseType
	{
		UnknownResponse = -1,
		Unauthorized = 0,
		Connected = 1,
		Closing = 2,
		FolderCreated = 3,
		FolderAlreadyExists = 4,
		Browse = 5,
		Rename = 6,
		RenameError = 7,
		DeletedFiles = 8,
		FileAlreadyExists = 9,
		BeginFileUpload = 10,
		FileUploading = 11,
		UploadCompleted = 12,
		BeginFileDownload = 13,
		DownloadComplete = 14,
		DownloadFolder = 15,
		DownloadFileError = 16,
	};

	enum class FileOverwrite
	{
		NoneSelected,
		OverwriteExisting,
		CreateNewFileName,
		SkipFile,
	};

	static QJsonArray createServerResponse(ResponseType responseStatus, const QString& dir, bool isBaseDir, quint64 bytesWritten = {});
	static QJsonArray createUploadProgressResponse(ResponseType responseStatus, const QString& path, quint64 bytesWritten);
	static bool checkIfBaseDir(const QString& directory, const QString& homeDirectory);
	static void deleteFiles(const QJsonArray& filesToDelete);
	static bool renameFile(const QString& filePath, const QString& oldFileName, QString& newFileName);
	static bool createFolder(const QString& newFolderName);
	static Transfer startFileUpload(const int& userIndex,const QString& fileName, const QString& filePath, const quint64& fileSize, const bool& baseDir, const QString& directoryToReturn);
	static Transfer createPendingFileDownload(const int& userIndex, const QString& filePath, const QString& fileName, const bool& baseDir, QString& errorString);
	static bool processFileDownload(const Transfer& download, QTcpSocket* socket);
	static quint64 processFileUpload(const QByteArray& data, Transfer& upload);
	static bool completeFileUpload(Transfer& upload);
	static void cancelFileUpload(Transfer& upload);
	static bool checkFileExists(const QString& filePath, const QString& fileName);
	static QString changeFileName(const QString& fileName, const QString& filePath);

private:
	static QJsonValue encodePixmapForJson(const QPixmap& p);
	static QPixmap getIconFromFileInfo(const QFileInfo& file);
	static QFileInfoList getFilesFromDirectory(const QString& dir, bool isBaseDir);

	FtpManager();
};

