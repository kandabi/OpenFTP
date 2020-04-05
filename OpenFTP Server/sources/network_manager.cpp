#include "stdafx.h"
#include "network_manager.h"

NetworkManager::NetworkManager(QMap<QUuid ,User>& _registeredUsers ,QWidget* parent) : QObject(parent) ,registeredUsers(_registeredUsers) {}

bool NetworkManager::initServer(const int& port)
{
	//sslServer.startServerEncryption();
	const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
	QHostAddress serverAddress;
	for (const QHostAddress& address : QNetworkInterface::allAddresses()) {
		if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
		{
			serverAddress = address;
			break;
		}
	}

	 bool isListening = server.listen(serverAddress, port);
	 if(isListening)
	 {
		 emit writeTextSignal("Server listening on address: " + serverAddress.toString() + " , port: " + QString::number(server.serverPort()), Qt::darkGreen);
		 return true;
	 }
	 else
	 {
		 emit writeTextSignal("Server failed to start.", Qt::darkRed);
		 return true;
	 }
}

bool NetworkManager::stopServer()
{
	for (User& user : connectedUsers)
	{
		QJsonArray serverResponse({ {"response_status" , static_cast<int>(FtpManager::ResponseType::Closing) } });
		writeToClient(user.getSocket(), Serializer::JsonToByteArray(serverResponse));
		disconnectUserSocket(user.getSocket());
	}

	for (Transfer& transfer : transfersInProgress)
	{
		emit writeTextSignal("Transfer: " + transfer.fileName + " aborted.", Qt::darkRed);
		transfer.cancelUpload();
	}

	transfersInProgress.clear();
	connectedUsers.clear();
	server.close();
	return true;
}

void NetworkManager::ForceUserDisconnect(QString userName)
{
	for(User& user : connectedUsers)
	{
		if (user.username == userName)
		{
			disconnectUserSocket(user.getSocket());
			deleteUserFromListSignal(user.username);
			connectedUsers.remove(user.getGuid());
			break;
		}
	}
}


void NetworkManager::parseJson(const QByteArray& data, QSslSocket* socket, const QUuid& userGuid)
{
	QJsonObject json = QJsonDocument::fromJson(data).object();
	User& currentUser = connectedUsers[userGuid];
	FtpManager::RequestType requestType = (FtpManager::RequestType)json.value("request_type").toInt();
	QString directory = json.value("requestPath").toString();
	FtpManager::ResponseType responseCode = FtpManager::ResponseType::UnknownResponse;

	QDir pathDir(directory);
	if (!pathDir.exists())
	{
		directory = currentUser.homeDirectory;
	}

	bool baseDir = FtpManager::checkIfBaseDir(directory, currentUser.homeDirectory);
	if (baseDir || !directory.contains(currentUser.homeDirectory))
	{
		directory = currentUser.homeDirectory;
	}

	switch (requestType)
	{
		case FtpManager::RequestType::Browse: { 
			responseCode = FtpManager::ResponseType::Browse;  
			break;  
		}
		case FtpManager::RequestType::Remove: {
			QJsonArray fileArray = json.value("filesToDelete").toArray();
			FtpManager::deleteFiles(fileArray);
			responseCode = FtpManager::ResponseType::DeletedFiles;
			std::for_each(fileArray.begin(), fileArray.end(), [this, &currentUser](QJsonValue const& file){
				emit writeTextSignal("Deleted item: " + file.toString() + ", from the server by user: " + currentUser.username, Qt::darkRed);
			});	
			break;
		}
		case FtpManager::RequestType::Rename: { 
			bool renamed = FtpManager::renameFile(directory, json.value("renameFile").toString(), json.value("changedFileName").toString()); 
			responseCode = (renamed) ? FtpManager::ResponseType::Rename : FtpManager::ResponseType::RenameError;
			break; 
		}
		case FtpManager::RequestType::CreateFolder: 
		{
			bool created = FtpManager::createFolder(json.value("createFolderPath").toString());
			responseCode = (created) ? FtpManager::ResponseType::FolderCreated : FtpManager::ResponseType::FolderAlreadyExists;
			emit writeTextSignal("Create folder request: " + json.value("createFolderPath").toString(), QColor(219, 143, 29));
			break;
		}
		case FtpManager::RequestType::UploadFile: 
		{
			QString fileName = json.value("fileName").toString();
			QString filePath = json.value("filePath").toString();
			FtpManager::FileOverwrite overwriteExistingFile = (FtpManager::FileOverwrite)json.value("overwrite").toString().toInt();
			emit writeTextSignal("Uploading file: " + fileName + ", File size: " + json.value("fileSize").toString() + " , by user: " + currentUser.username, QColor(219, 143, 29));

			if (FtpManager::checkFileExists(filePath, fileName))
			{
				if (overwriteExistingFile == FtpManager::FileOverwrite::SkipFile || overwriteExistingFile == FtpManager::FileOverwrite::NoneSelected)
				{
					responseCode = FtpManager::ResponseType::FileAlreadyExists;
					writeTextSignal("File " + fileName + " already exists on the server.", Qt::darkRed);
					break;
				}
				else if (overwriteExistingFile == FtpManager::FileOverwrite::CreateNewFileName)
				{
					fileName = FtpManager::changeFileName(fileName, filePath);
				}
			}

			responseCode = FtpManager::ResponseType::BeginFileUpload;
			Transfer fileTransfer = FtpManager::startFileUpload(userGuid, filePath, fileName, json.value("fileSize").toString().toULongLong(), baseDir, directory);
			transfersInProgress.insert(userGuid ,fileTransfer);
			currentUser.transferInProgress = true;
			
			break;
		}
		case FtpManager::RequestType::DownloadFile:
		{
			QString fileName = json.value("fileName").toString();
			transfersInProgress.remove(userGuid);
			
			QString errorString;
			Transfer download = FtpManager::createPendingFileDownload(userGuid, directory, fileName, baseDir, errorString);
			if (errorString.isEmpty())
			{
				transfersInProgress.insert(userGuid,download);
				responseCode = FtpManager::ResponseType::BeginFileDownload;
			}
			else
			{
				writeTextSignal(errorString, Qt::darkRed);
				responseCode = FtpManager::ResponseType::DownloadFileError;
			}
			break;
		}
		case FtpManager::RequestType::NextPendingDownload:
		{
			//int transferIndex = getTransferByUserIndex(userIndex);
			Transfer& download = transfersInProgress[userGuid];
			emit writeTextSignal("Download file request: " + download.fileName + ", File size: " + QString::number(download.fileSize) + ", by user: " + currentUser.username, QColor(219, 143, 29));

			if (download.fileSize < WorkerThread::filesizeToSplit)
			{
				bool result = FtpManager::processFileDownload(download, socket);
				Q_ASSERT(result);
			}
			else {
				download.workerThread = new WorkerThread(download.filePath + "/" + download.fileName, socket);
				connect(download.workerThread, &WorkerThread::finished, download.workerThread, &QObject::deleteLater);
				connect(download.workerThread, &WorkerThread::writeDataSignal, this, &NetworkManager::writeToClient);
				download.workerThread->start();
			}

			transfersInProgress.remove(userGuid);
			responseCode = FtpManager::ResponseType::DownloadComplete;
			break;
		}
		case FtpManager::RequestType::DownloadFolder:
		{
			QString fileName = json.value("fileName").toString();
			writeTextSignal("Download folder request: " + fileName, QColor(219, 143, 29));
			responseCode = FtpManager::ResponseType::DownloadFolder;
			baseDir = true;
			break;
		}
	}


	if (responseCode != FtpManager::ResponseType::DownloadComplete)
	{
		QJsonArray serverResponse = FtpManager::createServerResponse(responseCode, directory, baseDir);
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));
	}
}

void NetworkManager::parseUpload(const QByteArray& data, QSslSocket* socket, const QUuid& userGuid)
{
	//int transferIndex = getTransferByUserIndex(userIndex);
	//Q_ASSERT(transferIndex != -1);

	Transfer& currentUpload = transfersInProgress[userGuid];
	quint64 writtenBytes = FtpManager::processFileUpload(data, currentUpload);

	QJsonArray serverResponse;
	bool sendResponseData = false;

	if (currentUpload.fileSize <= currentUpload.writtenBytes)
	{
		bool result = currentUpload.finishUpload();
		Q_ASSERT(result);
		writeTextSignal("Upload complete: " + currentUpload.fileName, Qt::darkGreen);
		serverResponse = FtpManager::createServerResponse(FtpManager::ResponseType::UploadCompleted, currentUpload.directoryToReturn, currentUpload.isBaseDir, writtenBytes);
		socket->flush();
		transfersInProgress.remove(userGuid);
		sendResponseData = true;
		connectedUsers[userGuid].transferInProgress = false;

	}
	else {
		sendResponseData = (currentUpload.numOfPacketsSent++ % 10 == 0 || writtenBytes % (6000000) == 0) ? true : false;
		if (sendResponseData)
			serverResponse = FtpManager::createUploadProgressResponse(FtpManager::ResponseType::FileUploading, currentUpload.directoryToReturn, writtenBytes);
		
	}

	if(sendResponseData && !serverResponse.isEmpty())
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));
}

void NetworkManager::onReadyRead()
{
	QSslSocket* socket = getCurrentSocket();
	QUuid userGuid = getUserGuidBySocket(socket);
	Q_ASSERT(!userGuid.isNull());
	QByteArray data = socket->readAll();

	if (!connectedUsers[userGuid].transferInProgress)
	{
		parseJson(data, socket, userGuid);
	}
	else {
		parseUpload(data, socket, userGuid);
	}
}


void NetworkManager::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
	if (socketState == QAbstractSocket::UnconnectedState)
	{
		QSslSocket* socket = getCurrentSocket();
		QUuid& userGuid = getUserGuidBySocket(socket);
		if (connectedUsers[userGuid].transferInProgress)
		{
			if (transfersInProgress.contains(userGuid))
			{
				Transfer& transfer = transfersInProgress[userGuid];
				emit writeTextSignal("Transfer: " + transfer.fileName + " aborted by user: " + connectedUsers[userGuid].username, Qt::darkRed);
				transfer.cancelUpload();
				transfersInProgress.remove(userGuid);
			}
		}

		disconnectUserSocket(socket);

		if (!userGuid.isNull())
		{
			emit writeTextSignal("User " + connectedUsers[userGuid].username + " has disconnected", Qt::darkRed);
			emit deleteUserFromListSignal(connectedUsers[userGuid].username);

			connectedUsers.remove(userGuid);
		}
	}
}


void NetworkManager::newConnectionAttempt()
{
	emit writeTextSignal("New Connection Attempted", QColor(255, 153, 0));

	QSslSocket* socket = dynamic_cast<QSslSocket*>(server.nextPendingConnection());

	if (!socket->waitForReadyRead(10000))
	{
		socket->disconnectFromHost();;
	}

	isEncrypted(socket);

	QByteArray data = socket->readAll();
	QJsonObject userJson = QJsonDocument::fromJson(data).object();
	QUuid userGuid = validateUser(registeredUsers, userJson.value("username").toString(), userJson.value("password").toString());
	User connectedUser(userGuid, userJson.value("username").toString(), userJson.value("password").toString());

	bool alreadyConnected = false;

	if (!userGuid.isNull())
	{
		for (const User& user : connectedUsers)
			if (user.username == registeredUsers[userGuid].username)
				alreadyConnected = true;
	}

	
	if (!userGuid.isNull() && !alreadyConnected)
	{
		connectedUser.setSocket(socket);
		connectedUser.homeDirectory = registeredUsers[userGuid].homeDirectory;
		connectedUsers.insert(userGuid ,connectedUser);

		connect(socket, &QSslSocket::readyRead, this, &NetworkManager::onReadyRead);
		connect(socket, &QSslSocket::stateChanged, this, &NetworkManager::onSocketStateChanged);

		QJsonArray serverResponse = FtpManager::createServerResponse(FtpManager::ResponseType::Connected ,connectedUser.homeDirectory, true);
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));

		emit writeTextSignal("User " + connectedUser.username + " has connected.", Qt::darkGreen);
		emit connectUserToListSignal(connectedUser.username);
	}
	else
	{
		emit writeTextSignal("User Authentication Failed.", Qt::red);

		QJsonArray serverResponse({ {"response_status" , static_cast<int>(FtpManager::ResponseType::Unauthorized) } });
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));
		disconnectUserSocket(socket);
	}

}


QUuid NetworkManager::getUserGuidBySocket(QSslSocket* socket)
{
	for (const User& user : connectedUsers)
	{
		//if (socket->socketDescriptor() == connectedUsers[i].getSocketDescriptor());
		if(socket == user.getSocket())
		{
			//emit writeTextSignal("Socket descriptor: " + QString::number(socket->socketDescriptor()) + ", User Socket Descriptor: " + QString::number(connectedUsers[i].getSocketDescriptor()));
			return user.getGuid();
		}
	}
	return QUuid();
}


//int NetworkManager::getTransferByUserGuid(const QUuid& userGuid)
//{
//	for (int i = 0; i < transfersInProgress.count(); ++i)
//	{
//		Transfer& transfer = transfersInProgress[i];
//		if (transfer.userIndex == userIndex)
//			return i;
//	}
//
//	return -1;
//}

QSslSocket* NetworkManager::getCurrentSocket()
{
	return qobject_cast<QSslSocket*>(sender());
}


void NetworkManager::isEncrypted(const QSslSocket* socket)
{
	if(socket->isEncrypted())
		emit writeTextSignal("Socket is encrypted!", QColor(189, 121, 19));
	else 
		emit writeTextSignal("Socket is NOT encrypted!", Qt::darkRed);
}


bool NetworkManager::disconnectUserSocket(QSslSocket* socket)
{
	if (socket)
	{
		socket->disconnectFromHost();
		return socket->disconnect();
	}
	return false;
}


QUuid NetworkManager::validateUser(const QMap<QUuid ,User>& userList, QString name, QString password)
{
	if (name == "" && password == "")
		return QUuid();

	for (const User& user : userList)
	{
		if (user.username == name && user.password == password)
			return user.getGuid();
	}
	return QUuid();
}

bool NetworkManager::writeToClient(QSslSocket* socketToWrite, const QByteArray& data)
{
	return socketToWrite->write(data);
}


