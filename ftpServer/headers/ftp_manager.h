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
		Unauthorized,
		Connected,
		Closing,
		FolderCreated,
		FolderAlreadyExists,
		Browse,
		Rename,
		RenameError,
		DeletedFiles,
		FileAlreadyExists,
		BeginFileUpload,
		FileUploading,
		UploadCompleted,
		BeginFileDownload,
		DownloadComplete,
		DownloadFolder,
		DownloadFileError,
		UnknownResponse,
	};

	enum class FileOverwrite
	{
		NoneSelected,
		OverwriteExisting,
		CreateNewFileName,
		SkipFile,
	};

	static QJsonArray createServerResponse(ResponseType responseStatus, const QString& dir, bool isBaseDir, int bytesWritten = {});
	static QJsonArray createUploadProgressResponse(ResponseType responseStatus, const QString& path, int bytesWritten);
	static bool checkIfBaseDir(const QString& directory, const QString& homeDirectory);
	static void deleteFiles(const QJsonArray& filesToDelete);
	static bool renameFile(const QString& filePath, const QString& oldFileName, QString& newFileName);
	static bool createFolder(const QString& newFolderName);
	static Transfer startFileUpload(const int& userIndex,const QString& fileName, const QString& filePath, const int& fileSize, const bool& baseDir, const QString& directoryToReturn);
	static Transfer createPendingFileDownload(const int& userIndex, const QString& filePath, const QString& fileName, const bool& baseDir, QString& errorString);
	static bool beginFileDownload(const Transfer& download, QTcpSocket* socket, QString& errorString);
	static int processFileUpload(const QByteArray& data, Transfer& upload);
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

