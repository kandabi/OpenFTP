#include "./headers/stdafx.h"
#include "./headers/request_manager.h"



QJsonObject RequestManager::createServerRequest(RequestManager::RequestType action, const QMap<QString, QString>& requestVariables, const QStringList& deleteFiles)
{
	QJsonObject serverRequest
	{
		{ "request_type", static_cast<int>(action) },
		{ "requestPath", requestVariables["requestPath"]},
	};
	
	if (!deleteFiles.isEmpty())
	{
		QJsonArray jsonArray;
		for (const QString& pathToDelete : deleteFiles)
		{
			jsonArray.append(QJsonValue{ pathToDelete });
		}
		serverRequest.insert("filesToDelete", jsonArray);
	}
	else if (!requestVariables["renameFile"].isEmpty() && !requestVariables["changedFileName"].isEmpty())
	{
		serverRequest.insert("renameFile", requestVariables["renameFile"]);
		serverRequest.insert("changedFileName", requestVariables["changedFileName"]);
	}
	else if (!requestVariables["createFolderPath"].isEmpty())
	{
		serverRequest.insert("createFolderPath", requestVariables["createFolderPath"]);
	}
	else if (!requestVariables["uploadFileName"].isEmpty() && !requestVariables["uploadFileSize"].isEmpty() && !requestVariables["uploadOverwriteExisting"].isEmpty() && !requestVariables["uploadFilePath"].isEmpty() )
	{
		serverRequest.insert("fileName", requestVariables["uploadFileName"]);
		serverRequest.insert("fileSize", requestVariables["uploadFileSize"]);
		serverRequest.insert("overwrite", requestVariables["uploadOverwriteExisting"]);
		serverRequest.insert("filePath", requestVariables["uploadFilePath"]);
	}
	else if (!requestVariables["downloadFileName"].isEmpty())
	{
		serverRequest.insert("fileName", requestVariables["downloadFileName"]);
		
	}


	return serverRequest;
}


bool RequestManager::checkIfDataIsJson(const QByteArray& data)
{
	QJsonParseError jsonError;
	QJsonDocument::fromJson(data, &jsonError);
	return (jsonError.error == QJsonParseError::NoError) ? true : false;
}
