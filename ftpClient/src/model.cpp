#include "./headers/stdafx.h"
#include "./headers/model.h"


clientModel::clientModel(QWidget* parent) : QObject(parent), settingsManager(parent)
{
	QCoreApplication::setOrganizationName("kandabi");
	QCoreApplication::setOrganizationDomain("kandabi.com");
	QCoreApplication::setApplicationName("qtFtpServer");

	currentLocalDirectory = settingsManager.getDefaultBrowserDirectory();

	localBrowserModel = new QFileSystemModel(this);
	localBrowserModel->setFilter(QDir::NoDotDot | QDir::AllEntries);
	localBrowserModel->setRootPath(currentLocalDirectory);
}


void clientModel::init()
{
	connectionCredentials credentials = settingsManager.getConnectionCredentials();
	emit initClient(credentials.checkboxChecked, credentials.serverAddress, credentials.serverPort, credentials.userName, credentials.userPassword);

	emit setLocalFileBrowserSignal(*localBrowserModel);
	emit writeTextSignal("OpenFTP client 0.1.3, written by kandabi", Qt::darkGray);
}


void clientModel::connectToServer(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword,const bool& storeInformation)
{
	QHostAddress address(serverAddress);
	if (address.isNull())
	{
		emit writeTextSignal("Please enter a valid server IP address.", Qt::darkRed);
		return;
	}
	else if (serverPort.isEmpty())
	{
		emit writeTextSignal("Please enter a valid server port.", Qt::darkRed);
		return;
	}
	else if (userName.isEmpty())
	{
		emit writeTextSignal("Please enter your username assigned by the server admin.", Qt::darkRed);
		return;
	}
	else if (userPassword.isEmpty())
	{
		emit writeTextSignal("Please enter your password assigned by the server admin.", Qt::darkRed);
		return;
	}

	if (storeInformation)
		settingsManager.setConnectionCredentials(storeInformation ,serverAddress, serverPort, userName, userPassword);

	networkManager.connectToServer(serverAddress, serverPort, userName, userPassword);
}


void clientModel::saveConnectionCredentials(const bool& checkboxChecked, const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword)
{
	settingsManager.setConnectionCredentials(checkboxChecked ,serverAddress, serverPort, userName, userPassword);
}



void clientModel::parseJson(const QByteArray& data)
{
	bool isJson = RequestManager::checkIfDataIsJson(data);
	if (!isJson)
		return;

	QJsonParseError jsonError;
	QJsonArray jsonArray = QJsonDocument::fromJson(data, &jsonError).array();

	QJsonObject serverDetails = jsonArray.first().toObject();
	RequestManager::ResponseType responseType = static_cast<RequestManager::ResponseType>(serverDetails.value("response_status").toInt());

	if (responseType != RequestManager::ResponseType::DownloadFolder && responseType != RequestManager::ResponseType::BeginFileDownload && responseType != RequestManager::ResponseType::DownloadComplete)
		currentServerDirectory = serverDetails.value("directory").toString();

	switch (responseType)
	{
	case RequestManager::ResponseType::Connected:
		emit writeTextSignal("Succesfully connected to the FTP server, Address: " + networkManager.getSocketAddress() + "::" + networkManager.getSocketPort(), Qt::darkGreen);
		break;
	case RequestManager::ResponseType::FolderCreated:
		emit writeTextSignal("New folder created in: " + currentServerDirectory);
		checkRemainingUploads();
		break;
	case RequestManager::ResponseType::FolderAlreadyExists:
		checkRemainingUploads();
		break;
	case RequestManager::ResponseType::DeletedFiles:
		emit deletedFilesSignal();
		break;
	case RequestManager::ResponseType::Rename:
		break;
	case RequestManager::ResponseType::RenameError:
		beepSignal();
		break;
	case RequestManager::ResponseType::FileUploading:
		emit updateProgressBarSignal(serverDetails.value("bytesWritten").toInt());
		break;
	case RequestManager::ResponseType::FileAlreadyExists:
	{
		RequestManager::FileOverwrite overwriteBehaviour = (currentSessionFileBehavior != RequestManager::FileOverwrite::NoneSelected) ?
			currentSessionFileBehavior : settingsManager.getOverwriteExistingFileBehavior();

		if (overwriteBehaviour == RequestManager::FileOverwrite::SkipFile)
		{
			emit writeTextSignal("File: " + currentUpload.fileName() + " has been skipped.", Qt::darkRed);
			checkRemainingUploads();
		}
		else
			emit fileAlreadyExistsSignal(currentUpload.fileName());

		break;
	}
	case RequestManager::ResponseType::BeginFileUpload:
		networkManager.uploadFileData();
		break;
	case RequestManager::ResponseType::UploadCompleted:
		emit updateProgressBarSignal(serverDetails.value("bytesWritten").toInt());
		emit writeTextSignal("Upload complete: " + currentUpload.fileName(), Qt::darkGreen);
		checkRemainingUploads();
		break;
	case RequestManager::ResponseType::BeginFileDownload:
		beginPendingDownload(currentDownload, directoryToSave);
		break;
	case RequestManager::ResponseType::DownloadFolder: {
		QList<File> fileArray = FileManager::getFileListFromJson(jsonArray);

		writeTextSignal("Appended " + QString::number(fileArray.count()) + " extra files to download from directory: " + currentDownload.fileName);
		for (const File& file : fileArray)
		{
			fileListToDownload.prepend(file);
		}
		checkRemainingDownloads();
		break; }
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

	if (responseType != RequestManager::ResponseType::FileUploading && responseType != RequestManager::ResponseType::DownloadFolder &&
		responseType != RequestManager::ResponseType::BeginFileDownload && responseType != RequestManager::ResponseType::DownloadComplete)
	{
		serverFileList.clear();
		serverFileList = FileManager::getFileListFromJson(jsonArray);

		serverBrowserModel = new FileListServerModel(serverFileList, this);
		emit connectedToServerSignal(serverBrowserModel, currentServerDirectory);
	}
}



void clientModel::checkRemainingUploads()
{
	if (!fileListToUpload.isEmpty())
	{
		bool isDir = true;

		RequestManager::FileOverwrite overwriteBehaviour = (currentSessionFileBehavior != RequestManager::FileOverwrite::NoneSelected) ?
			currentSessionFileBehavior : settingsManager.getOverwriteExistingFileBehavior();

		for (int index = 0; index < fileListToUpload.count(); ++index)
		{
			currentUpload.setFile(fileListToUpload[index]);
			if (!currentUpload.isDir())
			{
				isDir = false;
				fileListToUpload.removeAt(index);
				break;
			}
		}

		if (isDir)
		{
			directoryToUpload = currentServerDirectory + currentUpload.filePath().split(currentLocalDirectory).last();
			fileListToUpload.removeLast();

			QDir directory(currentUpload.absoluteFilePath());
			QFileInfoList fileInfoList = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);

			for (const QFileInfo& file : fileInfoList)
			{
				fileListToUpload.prepend(file.absoluteFilePath());
			}

		}

		createUploadFileRequest(currentUpload, isDir, overwriteBehaviour);
	}
	else {
		directoryToUpload = "";
		emit uploadCompleteSignal();
	}
}



void clientModel::checkRemainingDownloads()
{
	networkManager.setdownloadInProgress(false);
	if (!fileListToDownload.isEmpty())
	{
		RequestManager::FileOverwrite overwriteBehaviour = (currentSessionFileBehavior != RequestManager::FileOverwrite::NoneSelected) ?
			currentSessionFileBehavior : settingsManager.getOverwriteExistingFileBehavior();

		for (int index = 0; index < fileListToDownload.count();)
		{
			currentDownload = fileListToDownload[index];

			if (!currentDownload.isDir)
			{
				fileListToDownload.removeAt(index);

				if (FileManager::checkFileExists(directoryToSave, currentDownload.fileName))
				{
					if (overwriteBehaviour == RequestManager::FileOverwrite::NoneSelected)
					{
						emit fileAlreadyExistsSignal(currentDownload.fileName);
						return;
					}
					else if (overwriteBehaviour == RequestManager::FileOverwrite::SkipFile)
					{
						emit writeTextSignal("File: " + currentDownload.fileName + " has been skipped.", Qt::darkRed);
						currentDownload = File();

						continue;
					}
				}
				else
					overwriteBehaviour = RequestManager::FileOverwrite::NoneSelected;

				break;
			}
			else {
				++index;
			}
		}

		if (currentDownload.isEmpty())
		{
			uploadCompleteSignal();
			directoryToSave.clear();
			return;
		}
		else if (currentDownload.isDir)
		{
			directoryToSave = currentLocalDirectory + currentDownload.filePath.split(currentServerDirectory).last();
			fileListToDownload.removeLast();
		}

		createDownloadFileRequest(currentDownload, overwriteBehaviour);

	}
	else {
		directoryToSave.clear();
		emit uploadCompleteSignal();
	}
}


void clientModel::createFolderAction(const QString& newFolderPath, bool createInServer)
{
	if (createInServer)
	{
		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory},
			{"createFolderPath" , newFolderPath}
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::CreateFolder, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		networkManager.writeData(data);
	}
	else
	{
		QDir dir;
		dir.mkpath(newFolderPath);
		emit setLocalFileBrowserSignal(*localBrowserModel);
	}
}


void clientModel::queueFilesToDownload(const QModelIndexList& indices, bool appendMorefiles)
{
	for (const QModelIndex& index : indices)
	{
		File file = serverFileList[index.row()];
		if (file.fileName == "." || file.fileName.contains(".lnk"))
			continue;

		fileListToDownload.append(file);
	}


	writeTextSignal("Appended " + QString::number(fileListToDownload.count()) + " files to download.");
	if (!appendMorefiles)
	{
		directoryToSave = currentLocalDirectory;
		checkRemainingDownloads();
	}
}


void clientModel::queueFilesToUpload(const QStringList& fileList, bool appendMorefiles)
{
	if (copiedServerFiles)
	{
		beepSignal();
		return;
	}

	emit writeTextSignal(QString::number(fileList.count()) + " Files added to upload queue.");
	fileListToUpload += fileList;

	if (!appendMorefiles)
	{
		directoryToUpload = currentServerDirectory;
		checkRemainingUploads();
		return;
	}
}


void clientModel::createUploadFileRequest(const QFileInfo& currentUpload, bool isDir, const RequestManager::FileOverwrite& overwriteOptionSelected)
{
	if (isDir) 
	{
		createFolderAction(directoryToUpload, true);
	}
	else {

		QFile qfile(currentUpload.absoluteFilePath());
		if (!qfile.open(QIODevice::ReadOnly))
		{
			emit uploadFailedSignal("Unable to load file, transfer failed. " + qfile.errorString());
			return;
		}

		QString fileSize = QString::number(qfile.size());
		RequestManager::FileOverwrite overwriteExisting = (overwriteOptionSelected == RequestManager::FileOverwrite::NoneSelected) ?
			settingsManager.getOverwriteExistingFileBehavior() : overwriteOptionSelected;
		emit writeTextSignal("Sending File: " + currentUpload.fileName() + " File Size: " + fileSize + " Directory to upload: " + directoryToUpload);
		emit networkManager.setProgressBarSignal(qfile.size());

		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory},
			{"uploadFileName",  currentUpload.fileName() },
			{"uploadFilePath", directoryToUpload},
			{"uploadFileSize", fileSize},
			{"uploadOverwriteExisting", QString::number(static_cast<int>(overwriteExisting)) },
		};

		QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::UploadFile, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(request);

		networkManager.writeData(data);
		networkManager.flushSocket();
	

		QByteArray fileData = qfile.readAll();
		if (fileData.isEmpty())
			fileData = " ";

		networkManager.setUploadDataToSend(fileData);
	}
}

void clientModel::createDownloadFileRequest(File& file, const RequestManager::FileOverwrite& overwriteOptionSelected)
{
	QString fileName = file.fileName;
	bool isFolder = file.isDir;
	QString requestPath = FileManager::getPreviousFolderPath(file.filePath);
	if (isFolder)
	{
		createFolderAction(directoryToSave, false);
		requestPath = file.filePath;
	}

	if (overwriteOptionSelected == RequestManager::FileOverwrite::CreateNewFileName)
		file.fileName = FileManager::changeFileName(fileName, directoryToSave);


	emit writeTextSignal("Downloading File: " + file.fileName + " File Size: " + file.fileSize + " Directory to upload: " + directoryToSave, Qt::darkGreen);

	QMap<QString, QString> requestVariables
	{
		{"requestPath", requestPath },
		{"downloadFileName", fileName},
	};

	RequestManager::RequestType requestType = (isFolder) ? RequestManager::RequestType::DownloadFolder : RequestManager::RequestType::DownloadFile;
	QJsonObject response = RequestManager::createServerRequest(requestType, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	networkManager.writeData(data);
}


void clientModel::renameInServer(const QModelIndex& index, const QString& newFileName)
{
	int rowSelected = index.row();
	QString fileName = serverFileList[rowSelected].fileName;

	if (fileName == ".")
		return;
	else if (FileManager::checkIfSensitiveDirectory(fileName))
	{
		emit beepSignal();
		writeTextSignal("Please stay away from sensitive directories :)", Qt::darkRed);
		return;
	}

	QMap<QString, QString> requestVariables{
		{"requestPath", currentServerDirectory},
		{"renameFile", fileName},
		{"changedFileName",  newFileName},
	};

	QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Rename, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	networkManager.writeData(data);
}


void clientModel::renameInLocal(const QString& oldFileName, QString& newFileName)
{
	QDir directory(currentLocalDirectory);
	QFileInfo file(directory, oldFileName);
	if (file.isFile() && !newFileName.contains("."))
	{
		newFileName += "." + file.suffix();
	}
	bool renameResult = directory.rename(oldFileName, newFileName);
}



void clientModel::browseHomeLocal()
{
	currentLocalDirectory = "";
	settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
	localBrowserModel->setRootPath(currentLocalDirectory);
	emit setLocalFileBrowserSignal(*localBrowserModel);
}


void clientModel::returnToLastFolderInLocal()
{
	currentLocalDirectory = FileManager::getPreviousFolderPath(currentLocalDirectory);
	settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
	localBrowserModel->setRootPath(currentLocalDirectory);
	emit setLocalFileBrowserSignal(*localBrowserModel);
}


void clientModel::returnToLastFolderInServer()
{
	if (!serverFileList.isEmpty())
	{

		QMap<QString, QString> requestVariables{
			{"requestPath", FileManager::getPreviousFolderPath(serverFileList[0].filePath) }
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		networkManager.writeData(data);
	}
}

void clientModel::browseHomeServer()
{
	QMap<QString, QString> requestVariables{
		{"requestPath", "./"}
	};

	QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(response);

	networkManager.writeData(data);
}




void clientModel::onDoubleClickLocalBrowser(const QModelIndex& index)
{
	const QModelIndex& selectedIndex = localBrowserModel->index(index.row(), 0, index.parent());
	bool result = localBrowserModel->isDir(selectedIndex);
	if (result)
	{
		QString fileName = localBrowserModel->fileName(selectedIndex);
		QString filePath = localBrowserModel->filePath(selectedIndex);

	if (FileManager::checkIfSensitiveDirectory(filePath))
	{
		emit beepSignal();
		writeTextSignal("Please stay away from sensitive directories :)", Qt::darkRed);
		return;
	}
			
			
		else if (fileName == ".")
			filePath = FileManager::getPreviousFolderPath(filePath);
		
		localBrowserModel->setRootPath(filePath);
		currentLocalDirectory = filePath;
		settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
		emit setLocalFileBrowserSignal(*localBrowserModel);
	}
}


void clientModel::onDoubleClickServerBrowser(const QModelIndex& index)
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

	networkManager.writeData(data);
}


void clientModel::deleteAction(const QModelIndexList& indices, bool deleteInServer)
{
	if (!deleteInServer)
	{
		for (int i = 0; i < indices.count(); ++i)
		{
			QString fileName = localBrowserModel->fileName(indices[i]);
			QString filePath = localBrowserModel->filePath(indices[i]);

			if (fileName == ".")
				continue;
			else if (FileManager::checkIfSensitiveDirectory(filePath))
			{
				emit beepSignal();
				writeTextSignal("Please stay away from sensitive directories :)", Qt::darkRed);
				continue;
			}
			else {
				QDir dir(filePath);
				dir.remove(filePath);
				dir.removeRecursively();
			}
		}

		emit deletedFilesSignal();
	}
	else if(!serverFileList[indices[0].row()].fileName.endsWith(":/Windows"))
	{
		QStringList deletePaths;
		//QString pathToRequest = FileManager::getPreviousFolderPath(fileList[indices[0].row()].filePath);
		if (serverFileList[indices[0].row()].fileName != ".")
			deletePaths.append(serverFileList[indices[0].row()].filePath);

		for (int i = 1; i < indices.count(); ++i)
		{
			deletePaths.append(serverFileList[indices[i].row()].filePath);
		}

		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory}
		};

		QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::Remove, requestVariables, deletePaths);
		QByteArray data = Serializer::JsonObjectToByteArray(request);

		networkManager.writeData(data);
	}
}


void clientModel::copyFilesToClipboardLocal(const QStringList& files)
{
	QList<QUrl> urlList;
	for (const QString& file : files)
	{
		urlList.append(QUrl::fromLocalFile(file));
	}

	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls(urlList);

	QApplication::clipboard()->setMimeData(mimeData);
	copiedServerFiles = false; 

	emit writeTextSignal(QString::number(files.count()) + " files has been copied to the clipboard.");
}


void clientModel::copyFilesToClipboardServer(const QModelIndexList& indices)
{
	QList<QUrl> urlList;
	for (int i = 0 ;i < indices.count();++i)
	{

		urlList.append(QUrl::fromLocalFile(serverFileList[indices[i].row()].filePath));
	}


	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls(urlList);

	QApplication::clipboard()->setMimeData(mimeData);
	copiedServerFiles = true;
	emit writeTextSignal(QString::number(indices.count()) + " files has been copied to the clipboard.");
}


void clientModel::copyFilesToDirectory(const QStringList& files, bool lastFunction, const QString& directoryTocopy)
{
	for (const QString& file : files)
	{ 
		if (QFileInfo(file).isDir())
		{
			QDir directory(file);
			QString newDirectoryTocopy = (directoryTocopy.isEmpty()) ? currentLocalDirectory + "/" + directory.dirName() : directoryTocopy + "/" + directory.dirName();
			
			
			QStringList extraFiles;
			QFileInfoList subdirectoryFiles = directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);
			directory.mkdir(newDirectoryTocopy);
			if (!subdirectoryFiles.isEmpty())
			{
				for (int i = 0; i < subdirectoryFiles.count(); ++i)
				{
					extraFiles.append(subdirectoryFiles[i].absoluteFilePath());
				}
				copyFilesToDirectory(extraFiles, false, newDirectoryTocopy);
			}
			
		}
		else {

			QString directory = (directoryTocopy.isEmpty()) ? currentLocalDirectory : directoryTocopy;
			QFile::copy(file, directory + "/" + QFileInfo(file).fileName());
		}
	}

	if(lastFunction)
		emit uploadCompleteSignal();
}


void clientModel::fileAlreadyExistsSelection(const int& selection, const bool& rememberSelectionForever, const bool& rememberTemporary)
{
	if (rememberSelectionForever)
		settingsManager.setOverwriteExistingFileBehavior(selection);
	else if (rememberTemporary)
		currentSessionFileBehavior = static_cast<RequestManager::FileOverwrite>(selection);

	auto overwriteFileSelection = static_cast<RequestManager::FileOverwrite>(selection);

	if (!currentDownload.isEmpty())
	{
		if (selection == 3)
		{
			emit writeTextSignal("File: " + currentDownload.fileName + " has been skipped.", Qt::darkRed);
			checkRemainingDownloads();
			return;
		}
		createDownloadFileRequest(currentDownload, overwriteFileSelection);
	}
	else if (currentUpload.exists())
	{
		if (selection == 3)
		{
			emit writeTextSignal("File: " + currentUpload.fileName() + " has been skipped.", Qt::darkRed);
			checkRemainingUploads();
			return;
		}
		createUploadFileRequest(currentUpload, false, overwriteFileSelection);
	}
	else
	{
		emit writeTextSignal("No file is queued for transfer.", Qt::darkRed);
		beepSignal();
	}
}


void clientModel::resetFileAlreadyExistsBehavior()
{
	settingsManager.setOverwriteExistingFileBehavior(0);
	currentSessionFileBehavior = RequestManager::FileOverwrite::NoneSelected;
	emit writeTextSignal("Succesfully reset to default behavior when transfering file that already exists.");
}


void clientModel::resetConnectionCredentials()
{
	settingsManager.setConnectionCredentials(false ,"",0, "", "");
	emit writeTextSignal("Succesfully removed saved connection data.");
}


void clientModel::searchFolder(const QString& directory, bool searchInServer)
{
	if (searchInServer)
	{
		QMap<QString, QString> requestVariables{
			{"requestPath", directory}
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		networkManager.writeData(data);
	}
	else {
		QDir pathDir(directory);
		if (pathDir.exists())
		{
			if (FileManager::checkIfSensitiveDirectory(directory))
			{
				emit beepSignal();
				writeTextSignal("Please stay away from sensitive directories :)", Qt::darkRed);
				return;
			}


			currentLocalDirectory = directory;
			settingsManager.setDefaultBrowserDirectory(currentLocalDirectory);
			localBrowserModel->setRootPath(currentLocalDirectory);
			emit setLocalFileBrowserSignal(*localBrowserModel);
		}
	}
}

void clientModel::requestServerUpdate()
{
	if (!currentServerDirectory.isEmpty() && !networkManager.isDownloading() && currentDownload.isEmpty() && !currentUpload.exists())
	{
		emit writeTextSignal("Fetching current server files.");

		QMap<QString, QString> requestVariables{
			{"requestPath", currentServerDirectory}
		};

		QJsonObject response = RequestManager::createServerRequest(RequestManager::RequestType::Browse, requestVariables);
		QByteArray data = Serializer::JsonObjectToByteArray(response);

		networkManager.writeData(data);
	}
}

void clientModel::beginPendingDownload(const File& currentDownload, const QString& directoryToSave)
{
	QMap<QString, QString> requestVariables{
		{"requestPath", currentDownload.filePath}
	};

	QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::NextPendingDownload, requestVariables);
	QByteArray data = Serializer::JsonObjectToByteArray(request);
	
	networkManager.beginPendingDownload(currentDownload, directoryToSave);
	networkManager.setdownloadInProgress(true);
	networkManager.writeData(data);
}

void clientModel::disconnectFromServerButton()
{
	networkManager.disconnectFromServer();
	copiedServerFiles = false;
}

void clientModel::disconnectedFromServer()
{
	serverFileList.clear(); 
}