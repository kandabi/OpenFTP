#include "./headers/stdafx.h"
#include "./headers/model.h"

ClientModel::ClientModel(QWidget* parent) : QObject(parent), settingsManager(parent)
{
	QCoreApplication::setOrganizationName("kandabi");
	QCoreApplication::setOrganizationDomain("kandabi.com");
	QCoreApplication::setApplicationName("qtFtpServer");

	currentLocalDirectory = settingsManager.getDefaultBrowserDirectory();

	localBrowserModel = new QFileSystemModel(this);
	localBrowserModel->setFilter( QDir::NoDotDot | QDir::AllEntries);
	localBrowserModel->setRootPath(currentLocalDirectory);
}


void ClientModel::init()
{
	emit writeTextSignal("kandabi ftp client 0.1.1", Qt::darkGray);
	emit setFileBrowserSignal(*localBrowserModel);
}


void ClientModel::connectToServer()
{
	emit writeTextSignal("Attempting connection to Ftp server...");

	socket.close();
	socket.connectToHost(QHostAddress("127.0.0.1"), 57184);
}


void ClientModel::disconnectFromServer()
{
	socket.close();
}


void ClientModel::onReadyRead()
{	

	QByteArray data = (downloadInProgress) ? socket.readAll() : parseByteData();
	
	if (data.isEmpty())
		return;

	if (downloadInProgress)
	{
		parseDownload(data);
	}
	else {
		parseJson(data);
	}
}

void ClientModel::parseDownload(const QByteArray& data)
{
	writtenBytes += qSaveFile.write(data);
	int fileSize = serverFileList[fileIndicesToDownload.first()].fileSize;
	emit updateProgressBarSignal(writtenBytes);

	if (writtenBytes >= fileSize)
	{
		qSaveFile.commit();
		checkRemainingDownloads();
	}
}


void ClientModel::parseJson(const QByteArray& data)
{
	//emit writeTextSignal("Packet size: " + QString::number(data.size()));

	bool isJson = RequestManager::checkIfDataIsJson(data);
	Q_ASSERT(isJson);

	QJsonParseError jsonError;
	QJsonArray jsonArray = QJsonDocument::fromJson(data, &jsonError).array();

	QJsonObject serverDetails = jsonArray.first().toObject();
	RequestManager::ResponseType responseType = static_cast<RequestManager::ResponseType>(serverDetails.value("response_status").toInt());
	currentServerDirectory = serverDetails.value("directory").toString();

	switch (responseType)
	{
	case RequestManager::ResponseType::Connected:
		emit writeTextSignal("Succesfully connected to the FTP server.", Qt::darkGreen);
		break;
	case RequestManager::ResponseType::FolderCreated:
		emit writeTextSignal("New folder created in: " + currentServerDirectory);
		checkRemainingUploads();
		break;
	case RequestManager::ResponseType::FolderAlreadyExists:
		checkRemainingUploads();
		//emit writeTextSignal("Folder already exists in: " + currentServerDirectory, Qt::darkRed);
		//emit beepSignal();
		break;
	case RequestManager::ResponseType::DeletedFiles:
		emit deletedFilesSignal();
		break;
	case RequestManager::ResponseType::FileUploading:
		emit updateProgressBarSignal(serverDetails.value("bytesWritten").toInt());
		break;
	case RequestManager::ResponseType::FileAlreadyExists:
		emit fileAlreadyExistsSignal(QFileInfo(fileNamesToUpload.first()).fileName());
		break;
	case RequestManager::ResponseType::BeginFileUpload:
		uploadFileData();
		break;
	case RequestManager::ResponseType::UploadCompleted:
		emit updateProgressBarSignal(serverDetails.value("bytesWritten").toInt());
		emit writeTextSignal("Upload complete: " + QFileInfo(fileNamesToUpload.first()).fileName(), Qt::darkGreen);
		checkRemainingUploads();
		break;
	case RequestManager::ResponseType::BeginFileDownload:
		beginPendingDownload(fileIndicesToDownload);
		break;
	//case RequestManager::ResponseType::DownloadComplete:
	case RequestManager::ResponseType::Unauthorized:
		emit writeTextSignal("Unauthorized server access.", Qt::darkRed);
		return;
	case RequestManager::ResponseType::Closing:
		emit writeTextSignal("Server shutting down, disconnecting.", Qt::darkRed);
		return;
	case RequestManager::ResponseType::UnknownResponse:
		emit writeTextSignal("Unknown server response.", Qt::darkRed);
		return;
	}
	
	serverFileList.clear();
	for (int i = 1; i < jsonArray.count(); ++i)
	{
		QJsonObject json = jsonArray[i].toObject();
		serverFileList.append(
			File{
				json.value("fileName").toString(),
				json.value("filePath").toString(),
				json.value("fileSize").toInt(),
				json.value("isDir").toBool(),
				json.value("lastModified").toString(),
				Serializer::decodePixmapFromString(json.value("icon").toString())
			}
		);
	}

	if (responseType != RequestManager::ResponseType::FileUploading)
	{
		serverBrowserModel = new FileListServerModel(serverFileList, this);
		emit connectedToServerSignal(serverBrowserModel, currentServerDirectory);
	}
}

void ClientModel::checkRemainingDownloads()
{
	downloadInProgress = false;
	if (fileIndicesToDownload.count() > 1)
	{
		
		fileIndicesToDownload.removeFirst();
		createDownloadRequest(fileIndicesToDownload);
	}
	else {
		fileIndicesToDownload.clear();
		emit uploadCompleteSignal();
	}
}


void ClientModel::checkRemainingUploads()
{
	if (fileNamesToUpload.count() > 1)
	{
		fileNamesToUpload.removeFirst();
		uploadFileRequest(fileNamesToUpload);
	}
	else {
		filePathToUpload = "";
		emit uploadCompleteSignal();
	}
}



void ClientModel::uploadFileData()
{
	socket.write(dataToSend);
}

void ClientModel::uploadFileRequest(const QStringList& fileList, bool appendMorefiles)
{
	if (appendMorefiles)
	{
		fileNamesToUpload += fileList;
		emit writeTextSignal(QString::number(fileList.count()) + " Files added to upload queue.");
		return;
	}

	fileNamesToUpload = fileList;
	QString firstFile = fileList.first();
	QDir directory(firstFile);

	if (directory.exists()) //*** Check if its a directory;
	{
		createFolderAction(filePathToUpload + directory.dirName(), true);

		QFileInfoList subdirectoryFiles = directory.entryInfoList();
		if (subdirectoryFiles.count() > 2)
		{

			for (int i = 2; i < subdirectoryFiles.count(); ++i)
			{
				fileNamesToUpload.insert(1, subdirectoryFiles[i].absoluteFilePath());

			}
			filePathToUpload += directory.dirName() + "/";
		}
	}
	else {
		QFile qfile(firstFile);

		if (!qfile.open(QIODevice::ReadOnly))
		{
			emit uploadFailedSignal("Unable to load file, transfer failed. " + qfile.errorString());
			//fileNamesToUpload.clear(); //*** remove this
			return;
		}

		QByteArray fileData = qfile.readAll();
		if (fileData.isEmpty())
			fileData = " ";
		QString fileName = QFileInfo(qfile.fileName()).fileName();
		QString filePath = (firstFile.contains(filePathToUpload)) ? currentServerDirectory + "/" + filePathToUpload : currentServerDirectory;
		QString fileSize = QString::number(qfile.size());

		emit writeTextSignal("Sending File: " + firstFile + " File Size: " + fileSize);

		emit setProgressBarSignal(qfile.size());
		emit updateProgressBarSignal(0);

		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory},
			{"uploadFileName", fileName},
			{"uploadFilePath", filePath},
			{"uploadFileSize", fileSize},
			{"uploadOverwriteExisting", QString::number(true)},
		};

		QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::UploadFile, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(request);

		socket.write(data);
		socket.flush();
		dataToSend = fileData;
	}
}


QByteArray ClientModel::parseByteData()
{
	QByteArray data = previousReadyReadData;
	data += socket.readAll();

	bool isJson = RequestManager::checkIfDataIsJson(data);
	if (isJson)
	{
		previousReadyReadData.clear();
	
	}
	else if (data.contains('[') && data.contains(']'))
	{
		QByteArrayList splitDataArray = data.split(']');
		data = splitDataArray.at(splitDataArray.count() - 2);
		data.append(']');

		bool isJson = RequestManager::checkIfDataIsJson(data);
		//Q_ASSERT(isJson);
	}
	else {
		previousReadyReadData = data;
		data = QByteArray();
	}
	
	return data;
}


void ClientModel::queueFilesToDownload(const QModelIndexList& indices)
{
	for (const QModelIndex& index : indices)
	{
		File file = serverFileList[index.row()];
		if (file.fileName == ".")
			continue;
		else if (file.isDir)
			continue; //*** Implementation Forthcoming

		fileIndicesToDownload.append(index.row());
	}

	createDownloadRequest(fileIndicesToDownload);
}


void ClientModel::createDownloadRequest(const QList<int>& fileList)
{
	QString fileName = serverFileList[fileList.first()].fileName;
	QMap<QString, QString> requestVariables
	{
		{"requestPath", currentServerDirectory},
		{"downloadFileName", fileName}
	};

	QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::DownloadFile, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	socket.write(data);
}


void ClientModel::beginPendingDownload(const QList<int>& fileList)
{
	QMap<QString, QString> requestVariables{
		{"requestPath", currentServerDirectory}
	};

	QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::NextPendingDownload, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(request);

	downloadInProgress = true;
	emit setProgressBarSignal(serverFileList[fileList.first()].fileSize);
	qSaveFile.setFileName(currentLocalDirectory + "/" + serverFileList[fileIndicesToDownload.first()].fileName);
	bool open = qSaveFile.open(QIODevice::WriteOnly);

	socket.write(data);
}


void ClientModel::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
	if (socketState == QAbstractSocket::ClosingState)
	{
		emit writeTextSignal("Disconnected from the server.", Qt::darkRed);
		socket.close();
		serverFileList.clear();
		emit disconnectedFromServerSignal();
	}
	else if (socketState == QAbstractSocket::ConnectedState)
	{
		QJsonObject request
		{
			{ "username", "aviv" },
			{ "password", "1234"},
		};

		socket.write(Serializer::JsonObjectToByteArray(request));
	}
}

void ClientModel::onDoubleClickLocalBrowser(const QModelIndex& index)
{	
	bool result = localBrowserModel->isDir(index);
	if (result)
	{
		QString fileName = localBrowserModel->fileName(index);
		QString filePath = localBrowserModel->filePath(index);

		if (checkIfSensitiveDirectory(filePath))
			return;

		else if (fileName == ".")
			filePath = FileManager::getPreviousFolderPath(filePath);
		
		localBrowserModel->setRootPath(filePath);
		currentLocalDirectory = filePath;
		settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
		emit setFileBrowserSignal(*localBrowserModel);
	}
}


void ClientModel::onDoubleClickServerBrowser(const QModelIndex& index)
{
	int rowSelected = index.row();
	if (!serverFileList[rowSelected].isDir)
		return;

	QString pathToRequest = (serverFileList[rowSelected].fileName == ".") ? FileManager::getPreviousFolderPath(serverFileList[rowSelected].filePath) : serverFileList[rowSelected].filePath;
	
	QMap<QString, QString> requestVariables{
		{"requestPath", pathToRequest}
	};
	
	QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(request);

	socket.write(data);
}


void ClientModel::deleteAction(const QModelIndexList& indices, bool deleteInServer)
{
	if (!deleteInServer)
	{

		for (int i = 0; i < indices.count(); ++i)
		{
			QString fileName = localBrowserModel->fileName(indices[i]);
			QString filePath = localBrowserModel->filePath(indices[i]);

			if (fileName == "." || checkIfSensitiveDirectory(filePath))
				continue;

			else {
				QDir dir(filePath);
				dir.remove(filePath);
				dir.removeRecursively();
			}
		}
	}
	else if(serverFileList[indices[0].row()].fileName != "." && !serverFileList[indices[0].row()].fileName.endsWith(":/Windows"))
	{
		QStringList deletePaths;
		//QString pathToRequest = FileManager::getPreviousFolderPath(fileList[indices[0].row()].filePath);
		for (int i = 0; i < indices.count(); ++i)
		{
			deletePaths.append(serverFileList[indices[i].row()].filePath);
		}

		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory}
		};

		QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::Remove, requestVariables, deletePaths);
		QByteArray data = Serializer::JsonObjectToByteArray(request);

		socket.write(data);
	}
}


void ClientModel::renameFile(const QModelIndex& index, const QString& newFileName)
{
	int rowSelected = index.row();
	QString fileName = serverFileList[rowSelected].fileName;

	if (fileName == "." || checkIfSensitiveDirectory(fileName))
		return;

	QMap<QString, QString> requestVariables{
		{"requestPath", currentServerDirectory},
		{"renameFile", fileName},
		{"changedFileName",  newFileName},
	};

	QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Rename, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	socket.write(data);
}

void ClientModel::createFolderAction(const QString& newFolderName, bool createInServer)
{

	if (createInServer)
	{
		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory},
			{"createFolderName" , newFolderName}
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::CreateFolder, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		socket.write(data);
	}
	else 
	{
		QDir(currentLocalDirectory).mkdir(newFolderName);
		emit setFileBrowserSignal(*localBrowserModel);
	}


}

void ClientModel::searchFolder(const QString& directory, bool searchInServer)
{
	if (searchInServer)
	{
		QMap<QString, QString> requestVariables{
			{"requestPath", directory}
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		socket.write(data);
	}
	else {
		QDir pathDir(directory);
		if (pathDir.exists())
		{
			if (checkIfSensitiveDirectory(directory))
				return;

			currentLocalDirectory = directory;
			settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
			localBrowserModel->setRootPath(currentLocalDirectory);
			emit setFileBrowserSignal(*localBrowserModel);
		}
	}

	//emit writeTextSignal("searching: " + directory + " in server: " + QString::number(searchInServer));
}

bool ClientModel::checkIfSensitiveDirectory(const QString& directory)
{
	QDir pathDir(directory);
	if (pathDir.path().contains(":/Windows"))
	{
		emit beepSignal();
		writeTextSignal("Please stay away from sensitive directories :)", Qt::darkRed);
		return true;
	}
	else
		return false;

}



void ClientModel::localBrowseHome()
{
	currentLocalDirectory = "";
	settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
	localBrowserModel->setRootPath(currentLocalDirectory);
	emit setFileBrowserSignal(*localBrowserModel);
}

void ClientModel::browseHome()
{
	QMap<QString, QString> requestVariables{ 
		{"requestPath", "./"} 
	};

	QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	socket.write(data);
}

void ClientModel::localReturnToLastFolder()
{
	currentLocalDirectory = FileManager::getPreviousFolderPath(currentLocalDirectory);
	settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
	localBrowserModel->setRootPath(currentLocalDirectory);
	emit setFileBrowserSignal(*localBrowserModel);
}



void ClientModel::returnToLastFolder()
{
	if (!serverFileList.isEmpty())
	{

		QMap<QString, QString> requestVariables{
			{"requestPath", FileManager::getPreviousFolderPath(serverFileList[0].filePath) }
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		socket.write(data);
	}
}

