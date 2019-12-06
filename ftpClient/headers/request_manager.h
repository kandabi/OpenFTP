#pragma once
#include "stdafx.h"

class RequestManager
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

	static QJsonObject createServerRequest(RequestManager::RequestType action, const QMap<QString, QString>& requestVariables, const QStringList& deleteFiles = {});
	static bool checkIfDataIsJson(const QByteArray& data);

private:
	RequestManager();
};

