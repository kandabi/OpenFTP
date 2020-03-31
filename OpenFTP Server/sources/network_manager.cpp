#include "stdafx.h"
#include "network_manager.h"

NetworkManager::NetworkManager(QList<User>& _registeredUsersList ,QWidget* parent) : QObject(parent) ,registeredUsersList(_registeredUsersList) {}

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

	//serverAddress = QHostAddress::LocalHost;
	//bool certLoaded = server.setSslLocalCertificate("sslserver.pem");
	//bool keyLoaded = server.setSslPrivateKey("sslserver.key");
	//server.setSslProtocol(QSsl::TlsV1_2);

	 bool isListening = server.listen(serverAddress, port);
	 //if (isListening && certLoaded && keyLoaded)
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
		transfer.cancelUpload();
	}

	transfersInProgress.clear();
	connectedUsers.clear();
	server.close();
	return true;
}

void NetworkManager::ForceUserDisconnect(QString userName)
{
	for (int i = 0; i < connectedUsers.count(); ++i)
	{
		if (connectedUsers[i].username == userName)
		{
			disconnectUserSocket(connectedUsers[i].getSocket());
			deleteUserFromListSignal(connectedUsers[i].username);
			connectedUsers.removeAt(i);
			break;
		}
	}
}


void NetworkManager::parseJson(const QByteArray& data, QSslSocket* socket, int userIndex)
{
	QJsonObject json = QJsonDocument::fromJson(data).object();
	User& currentUser = connectedUsers[userIndex];
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
			Transfer fileTransfer = FtpManager::startFileUpload(userIndex, filePath, fileName, json.value("fileSize").toString().toULongLong(), baseDir, directory);
			transfersInProgress.append(fileTransfer);
			currentUser.transferInProgress = true;
			
			break;
		}
		case FtpManager::RequestType::DownloadFile:
		{
			QString fileName = json.value("fileName").toString();
			int currentTransfer = getTransferByUserIndex(userIndex);
			if (currentTransfer != -1)
				transfersInProgress.removeAt(currentTransfer);
			
			QString errorString;
			Transfer download = FtpManager::createPendingFileDownload(userIndex, directory, fileName, baseDir, errorString);
			if (errorString.isEmpty())
			{
				transfersInProgress.append(download);
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
			int transferIndex = getTransferByUserIndex(userIndex);
			Transfer& download = transfersInProgress[transferIndex];
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

			transfersInProgress.removeAt(transferIndex);
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

void NetworkManager::parseUpload(const QByteArray& data, QSslSocket* socket, int userIndex)
{
	int transferIndex = getTransferByUserIndex(userIndex);
	Q_ASSERT(transferIndex != -1);

	Transfer& currentUpload = transfersInProgress[transferIndex];
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
		transfersInProgress.removeAt(transferIndex);
		sendResponseData = true;
		connectedUsers[userIndex].transferInProgress = false;

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
	int userIndex = getUserIndexBySocket(socket);
	Q_ASSERT(userIndex != -1);
	QByteArray data = socket->readAll();

	if (!connectedUsers[userIndex].transferInProgress)
	{
		parseJson(data, socket, userIndex);
	}
	else {
		parseUpload(data, socket, userIndex);
	}
}


void NetworkManager::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
	if (socketState == QAbstractSocket::UnconnectedState)
	{
		QSslSocket* socket = getCurrentSocket();
		int userIndex = getUserIndexBySocket(socket);
		if (connectedUsers[userIndex].transferInProgress)
		{
			int index = getTransferByUserIndex(userIndex);
			if (index != -1)
			{
				Transfer& transfer = transfersInProgress[index];
				transfer.cancelUpload();
				transfersInProgress.removeAt(index);
			}
		}

		disconnectUserSocket(socket);

		if (userIndex != -1)
		{
			emit writeTextSignal("User " + connectedUsers[userIndex].username + " has disconnected", Qt::darkRed);
			emit deleteUserFromListSignal(connectedUsers[userIndex].username);

			connectedUsers.removeAt(userIndex);
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
	User connectedUser(userJson.value("username").toString(), userJson.value("password").toString());
	int userIndex = validateUser(registeredUsersList, connectedUser.username, connectedUser.password);
	bool alreadyConnected = false;

	if (userIndex != -1)
	{
		for (const User& user : connectedUsers)
			if (user.username == registeredUsersList[userIndex].username)
				alreadyConnected = true;
	}

	
	if (userIndex != -1 && !alreadyConnected)
	{
		connectedUser.setSocket(socket);
		connectedUser.homeDirectory = registeredUsersList[userIndex].homeDirectory;
		connectedUsers.append(connectedUser);

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


int NetworkManager::getUserIndexBySocket(QSslSocket* socket)
{
	for (int i = 0; i < connectedUsers.count(); ++i)
	{
		//if (socket->socketDescriptor() == connectedUsers[i].getSocketDescriptor());
		if(socket == connectedUsers[i].getSocket())
		{
			//emit writeTextSignal("Socket descriptor: " + QString::number(socket->socketDescriptor()) + ", User Socket Descriptor: " + QString::number(connectedUsers[i].getSocketDescriptor()));
			return i;
		}
	}
	return -1;
}


int NetworkManager::getTransferByUserIndex(const int& userIndex)
{
	for (int i = 0; i < transfersInProgress.count(); ++i)
	{
		Transfer& transfer = transfersInProgress[i];
		if (transfer.userIndex == userIndex)
			return i;
	}

	return -1;
}

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


int NetworkManager::validateUser(const QList<User>& userList, QString name, QString password)
{
	if (name == "" && password == "")
		return -1;

	for (int i = 0; i < userList.count(); ++i)
	{
		if (userList[i].username == name && userList[i].password == password)
			return i;
	}
	return -1;
}

bool NetworkManager::writeToClient(QSslSocket* socketToWrite, const QByteArray& data)
{
	return socketToWrite->write(data);
}

