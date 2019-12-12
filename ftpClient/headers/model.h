#pragma once
#include "stdafx.h"
#include "serialization_manager.h"
#include "user.h"
#include "list_model.h"
#include "file.h"
#include "file_manager.h"
#include "request_manager.h"
#include "settings_manager.h"

class ClientModel : public QObject
{
	Q_OBJECT
	friend class clientController;
public:
	ClientModel(QWidget* parent = Q_NULLPTR);
	void init();

signals:
	void writeTextSignal(QString text, QColor color = Qt::black);
	void connectedToServerSignal(FileListServerModel* model, const QString& currentDirectory);
	void disconnectedFromServerSignal();
	void updateProgressBarSignal(qint64 bytesReceived);
	void setProgressBarSignal(qint64 bytesTotal);
	void uploadCompleteSignal();
	void uploadFailedSignal(QString errorString);
	void setLocalFileBrowserSignal(QFileSystemModel& model);
	void beepSignal();
	void fileAlreadyExistsSignal(const QString& filename);
	void deletedFilesSignal();

private slots:
	void connectToServer();
	void disconnectFromServer();
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);
	void onDoubleClickServerBrowser(const QModelIndex& index);
	void onDoubleClickLocalBrowser(const QModelIndex& index);
	void deleteAction(const QModelIndexList& indices, bool deleteInServer);
	void renameInServer(const QModelIndex& indices, const QString& newFileName);
	void renameInLocal(const QString& oldFileName, QString& newFileName);
	void createFolderAction(const QString& newFolderPath, bool createInServer);
	void browseHome();
	void localBrowseHome();
	void returnToLastFolder();
	void localReturnToLastFolder();
	void uploadFileRequest(const QFileInfo& currentUpload, bool isDir, const RequestManager::FileOverwrite& overwriteOptionSelected = RequestManager::FileOverwrite::NoneSelected);
	void searchFolder(const QString& directory, bool searchInServer);
	void queueFilesToDownload(const QModelIndexList& indices, bool appendMorefiles);
	void queueFilesToUpload(const QStringList& fileList, bool appendMorefiles);
	void copyFilesToDirectory(const QStringList& files, bool lastFunction, const QString& directoryTocopy = {});
	void copyFilesToClipboardLocal(const QStringList& files);
	void copyFilesToClipboardServer(const QModelIndexList& files);
	void fileAlreadyExistsSelection(const int& selection, const bool& rememberSelectionForever, const bool& rememberTemporary);
	void resetFileAlreadyExistsBehavior();
	
	//void updateProgressBar(qint64 bytesReceived);
	//void setProgressBar(qint64 bytesTotal);

	//void connectionEstablished();
	//void updateData();

private:
	QByteArray parseByteData();
	void uploadFileData();
	void checkRemainingUploads();
	void checkRemainingDownloads();
	void downloadFileRequest(File& file, const RequestManager::FileOverwrite& overwriteOptionSelected = RequestManager::FileOverwrite::NoneSelected);
	void beginPendingDownload(const File& file);
	void parseDownload(const QByteArray& data);
	void parseJson(const QByteArray& jsonArray);
	bool checkIfSensitiveDirectory(const QString& pathDir);
	QList<File> getFilesListFromJson(const QJsonArray& jsonArray);
	//void parseFolderFiles();
	

	QString directoryToUpload;
	QString currentLocalDirectory;
	QString currentServerDirectory;
	
	QFileSystemModel* localBrowserModel;
	FileListServerModel* serverBrowserModel;
	SettingsManager settingsManager;
	QTcpSocket socket;
	QSaveFile qSaveFile;

	QByteArray previousReadyReadData;
	QByteArray dataToSend;

	QList<File> serverFileList;
	QStringList fileListToUpload;
	QList<File> fileListToDownload;
	File currentDownload;
	QFileInfo currentUpload;
	QString directoryToSave;
	
	bool copiedServerFiles = false;
	bool downloadInProgress = false;
	int writtenBytes = 0;
	RequestManager::FileOverwrite currentSessionFileBehavior = RequestManager::FileOverwrite::NoneSelected;

};
