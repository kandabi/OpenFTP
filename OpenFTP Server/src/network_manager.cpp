#include "stdafx.h"
#include "network_manager.h"


NetworkManager::NetworkManager(QList<User>& _registeredUsersList ,QWidget* parent) : QObject(parent) ,registeredUsersList(_registeredUsersList)
{
}

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
		QJsonArray serverResponse;
		serverResponse.append(QJsonObject{ {"response_status" , static_cast<int>(FtpManager::ResponseType::Closing) } });
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

void NetworkManager::disconnectUser(QString userName)
{
	for (int i = 0; i < connectedUsers.count(); ++i)
	{
		if (connectedUsers[i].username == userName)
		{
			disconnectUserSocket(connectedUsers[i].getSocket());
			break;
		}
	}
}


void NetworkManager::parseJson(const QByteArray& data, QTcpSocket* socket, int userIndex)
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
			FtpManager::deleteFiles(json.value("filesToDelete").toArray());
			responseCode = FtpManager::ResponseType::DeletedFiles;
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
			break;
		}
		case FtpManager::RequestType::UploadFile: 
		{
			QString fileName = json.value("fileName").toString();
			QString filePath = json.value("filePath").toString();
			FtpManager::FileOverwrite overwriteExistingFile = (FtpManager::FileOverwrite)json.value("overwrite").toString().toInt();
			emit writeTextSignal("Uploading file: " + fileName + ", File size: " + json.value("fileSize").toString() + " , by username: " + currentUser.username);

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
			QString errorString;
			int transferIndex = getTransferByUserIndex(userIndex);
			Transfer& download = transfersInProgress[transferIndex];
			emit writeTextSignal("Download file request: " + download.fileName + ", File size: " + QString::number(download.fileSize) + ", by username: " + currentUser.username);
			bool result = FtpManager::beginFileDownload(download, socket, errorString);
			Q_ASSERT(result);
			transfersInProgress.removeAt(transferIndex);
			responseCode = FtpManager::ResponseType::DownloadComplete;
			break;
		}
		case FtpManager::RequestType::DownloadFolder:
		{
			QString fileName = json.value("fileName").toString();
			writeTextSignal("Download folder request: " + fileName);
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

void NetworkManager::parseUpload(const QByteArray& data, QTcpSocket* socket, int userIndex)
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
		serverResponse = FtpManager::createUploadProgressResponse(FtpManager::ResponseType::FileUploading, currentUpload.directoryToReturn, writtenBytes);
		sendResponseData = (currentUpload.numOfPacketsSent % 10 == 0) ? true : false;
		currentUpload.numOfPacketsSent++;
	}

	if(!serverResponse.isEmpty() && sendResponseData)
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));
}

void NetworkManager::onReadyRead()
{
	QTcpSocket* socket = NetworkManager::getCurrentSocket();
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
		QTcpSocket* socket = NetworkManager::getCurrentSocket();
		int userIndex = getUserIndexBySocket(socket);
		if (connectedUsers[userIndex].transferInProgress)
		{
			int index = getTransferByUserIndex(userIndex);
			if (index != -1)
			{
				Transfer& transfer = transfersInProgress[index];
				transfer.cancelUpload();
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

	QTcpSocket* socket = server.nextPendingConnection();

	//QSslSocket* socket = dynamic_cast<QSslSocket*>(server.nextPendingConnection());
	//if(!socket->waitForEncrypted(1000))
	//{
	//	socket->disconnectFromHost();;
	//}

	if (!socket->waitForReadyRead(1000))
	{
		socket->disconnectFromHost();
	}

	auto data = socket->readAll();

	QJsonObject userJson = QJsonDocument::fromJson(data).object();
	User connectedUser(userJson.value("username").toString(), userJson.value("password").toString());
	int userIndex = validateUser(registeredUsersList, connectedUser.username, connectedUser.password);
	bool alreadyConnected = false;
	for (const User& user : connectedUsers)
		if (user.username == registeredUsersList[userIndex].username)
			alreadyConnected = true;

	if (userIndex != -1 && !alreadyConnected)
	{
		connectedUser.setSocket(socket);
		connectedUser.homeDirectory = registeredUsersList[userIndex].homeDirectory;
		connectedUsers.append(connectedUser);

		connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
		connect(socket, &QTcpSocket::stateChanged, this, &NetworkManager::onSocketStateChanged);

		QJsonArray serverResponse = FtpManager::createServerResponse(FtpManager::ResponseType::Connected ,connectedUser.homeDirectory, true);
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));

		emit writeTextSignal("User " + connectedUser.username + " has connected.", Qt::darkGreen);
		emit connectUserToListSignal(connectedUser.username);
	}
	else
	{
		emit writeTextSignal("User Authentication Failed.", Qt::red);

		QJsonArray serverResponse;
		serverResponse.append(QJsonObject{ {"response_status" , static_cast<int>(FtpManager::ResponseType::Unauthorized) } });
		writeToClient(socket, Serializer::JsonToByteArray(serverResponse));
		disconnectUserSocket(socket);
	}
}


bool NetworkManager::disconnectUserSocket(QTcpSocket* socket)
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

bool NetworkManager::writeToClient(QTcpSocket* socketToWrite, const QByteArray& data)
{
	return socketToWrite->write(data);
}


QTcpSocket* NetworkManager::getCurrentSocket()
{
	return qobject_cast<QTcpSocket*>(sender());
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


int NetworkManager::getUserIndexBySocket(QTcpSocket* socket)
{
	for (int i = 0; i < connectedUsers.count(); ++i)
	{
		if (socket == connectedUsers[i].getSocket());
		{
			return i;
		}
	}
	return -1;
}
