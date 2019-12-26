#pragma once
#include "stdafx.h"
#include "file_manager.h"
#include "request_manager.h"

class NetworkManager : public QObject
{
	Q_OBJECT
	friend class clientController;
public:
	NetworkManager(QWidget* parent = Q_NULLPTR);

	void connectToServer(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	void disconnectFromServer();
	void uploadFileData();
	void setUploadDataToSend(const QByteArray& data);

	QByteArray parseByteData();
	void parseDownload(const QByteArray& data);
	void beginPendingDownload(const File& currentDownload, const QString& directoryToSave);

	void writeData(const QByteArray& data);
	void flushSocket();
	QByteArray readAll();

	void setdownloadInProgress(const bool& download);
	inline bool isDownloading()
	{
		return downloadInProgress;
	}

	QString getSocketAddress();
	QString getSocketPort();

signals:
	void writeTextSignal(QString text, QColor color = Qt::white);
	void disconnectedFromServerSignal();
	void parseJsonSignal(const QByteArray& jsonArray);
	void updateProgressBarSignal(qint64 bytesReceived);
	void setProgressBarSignal(qint64 bytesTotal);
	void checkRemainingDownloadsSignal();

public slots:
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
	QTcpSocket socket;
	//QSslSocket socket;

	QSaveFile qSaveFile;

	bool downloadInProgress = false;

	int numOfPacketsRecieved = 0;
	int writtenBytes = 0;
	int currentDownloadFileSize;

	QByteArray previousReadyReadData;
	QByteArray dataToSend;

	QString username;
	QString password;
};