#pragma once
#include "stdafx.h"
#include "network_manager.h"
#include "settings_manager.h"
#include "list_model.h"
#include "worker_manager.h"
#include "logger_manager.h"


class clientModel : public QObject
{
	Q_OBJECT
	friend class clientController;
public:
	clientModel(QWidget* parent = Q_NULLPTR);
	void init();

signals:
	void writeTextSignal(QString text, QColor color = Qt::white);
	void beepSignal();
	
	void uploadCompleteSignal();
	void uploadFailedSignal(QString errorString);
	void connectedToServerSignal(FileListServerModel* model, const QString& currentDirectory, const QModelIndexList fileIndicesToSelect);
	void updateProgressBarSignal(quint64 bytesReceived, quint64 totalBytes);
	void setLocalFileBrowserSignal(QFileSystemModel& model);
	void fileAlreadyExistsSignal(const QString& filename);
	void deletedFilesSignal();
	void connectToServerSignal(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	void initClient(const bool& storeCredentials ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword, const bool& minimizeToTray);


private slots:
	void connectToServer(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword, const bool& saveInformation);
	void disconnectFromServerButton();
	void disconnectedFromServer();
	void checkRemainingUploads();
	void checkRemainingDownloads();
	void queueFilesToDownload(const QModelIndexList& indices, bool appendMorefiles);
	void queueFilesToUpload(const QStringList& fileList, bool appendMorefiles);
	void createUploadFileRequest(const QFileInfo& currentUpload, bool isDir, const RequestManager::FileOverwrite& overwriteOptionSelected = RequestManager::FileOverwrite::NoneSelected, const bool& writeText = true);
	void createDownloadFileRequest(File& file, const RequestManager::FileOverwrite& overwriteOptionSelected = RequestManager::FileOverwrite::NoneSelected, const bool& writeText = true);
	void createFolderAction(const QString& newFolderPath, bool createInServer);
	void renameInServer(const QModelIndex& indices, const QString& newFileName);
	void renameInLocal(const QString& oldFileName, QString& newFileName);
	void browseHomeServer();
	void browseHomeLocal();
	void returnToLastFolderInServer();
	void returnToLastFolderInLocal();
	void onDoubleClickServerBrowser(const QModelIndex& index);
	void onDoubleClickLocalBrowser(const QModelIndex& index);
	void deleteAction(const QModelIndexList& indices, bool deleteInServer);
	void copyFilesToDirectory(const QStringList& files, bool lastFunction, const QString& directoryTocopy = {});
	void copyFilesToClipboardLocal(const QStringList& files);
	void copyFilesToClipboardServer(const QModelIndexList& files);
	void fileAlreadyExistsSelection(const int& selection, const bool& rememberSelectionForever, const bool& rememberTemporary);
	void resetFileAlreadyExistsBehavior();
	void resetConnectionCredentials();
	void saveConnectionCredentials(const bool& checkboxChecked, const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	void searchFolder(const QString& directory, bool searchInServer);
	void refreshServerBrowser(const QModelIndexList selected);
	void setMinimizeToTray(bool checked);
	void cancelTransfers();

private:
	void beginPendingDownload(const File& currentDownload, const QString& directoryToSave);
	void parseJson(const QByteArray& jsonArray);
	void createWorkerThread(const quint64& readFromPosition = 0/*, const QByteArray& requestData*/);

	SettingsManager settingsManager;
	NetworkManager networkManager;
	LoggerManager logger;

	QString currentLocalDirectory;
	QString currentServerDirectory;

	QFileInfo currentUpload;
	File currentDownload;

	QString directoryToUpload;
	QString directoryToSave;

	QStringList fileListToUpload;
	QModelIndexList fileIndicesToSelect;
	QList<File> fileListToDownload;
	QList<File> serverFileList;

	bool copiedServerFiles = false;

	QFileSystemModel* localBrowserModel;
	FileListServerModel* serverBrowserModel;

	RequestManager::FileOverwrite currentSessionFileBehavior = RequestManager::FileOverwrite::NoneSelected;

	WorkerThread* workerThread;
	QByteArray requestDataForMultithreading;
};

