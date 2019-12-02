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
	void setFileBrowserSignal(QFileSystemModel& model);
	void beepSignal();
	void fileAlreadyExistsSignal(QString filename);
	void deletedFilesSignal();

private slots:
	void connectToServer();
	void disconnectFromServer();
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);
	void onDoubleClickServerBrowser(const QModelIndex& index);
	void onDoubleClickLocalBrowser(const QModelIndex& index);
	void deleteAction(const QModelIndexList& indices, bool deleteInServer);
	void renameFile(const QModelIndex& indices, const QString& newFileName);
	void createFolderAction(const QString& newFolderName, bool createInServer);
	void browseHome();
	void localBrowseHome();
	void returnToLastFolder();
	void localReturnToLastFolder();
	void uploadFileRequest(const QStringList& fileList, bool appendMoreFiles = false);
	void searchFolder(const QString& directory, bool searchInServer);
	void queueFilesToDownload(const QModelIndexList& indices);

	//void updateProgressBar(qint64 bytesReceived);
	//void setProgressBar(qint64 bytesTotal);

	//void connectionEstablished();
	//void updateData();

private:
	QByteArray parseByteData();
	void uploadFileData();
	void checkRemainingUploads();
	void checkRemainingDownloads();
	void createDownloadRequest(const QList<int>& fileListToDownload);
	void beginPendingDownload(const QList<int>& fileList);
	void parseDownload(const QByteArray& data);
	void parseJson(const QByteArray& jsonArray);
	bool checkIfSensitiveDirectory(const QString& pathDir);
	

	QString filePathToUpload;
	QString currentLocalDirectory;
	QString currentServerDirectory;
	
	QFileSystemModel* localBrowserModel;
	FileListServerModel* serverBrowserModel;
	SettingsManager settingsManager;
	QTcpSocket socket;
	QSaveFile qSaveFile;

	QByteArray previousReadyReadData;
	QByteArray dataToSend;

	QVector<File> serverFileList;
	QStringList fileNamesToUpload;
	QList<int> fileIndicesToDownload;
	
	bool downloadInProgress = false;
	int writtenBytes = 0;
};

