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
	QByteArray parseByteData();
	void parseByteDownload(const QByteArray& data);
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
	void updateProgressBarSignal(quint64 bytesReceived, quint64 bytesTotal);
	void setProgressBarSignal();
	void checkRemainingDownloadsSignal();
	//void createWorkerThreadSignal(const quint64& readFromPosition);

public slots:
	void setUploadDataToSend(const QByteArray& data);
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);
	//void writeDataAsync(const QByteArray& data);

public:
	bool useWorkerThread = false;
	int packetsSent = 0;

private:
	QTcpSocket socket;
	QSaveFile qSaveFile;
	//QSslSocket socket;

	bool downloadInProgress = false;

	int packetsRecieved = 0;
	int writtenBytes = 0;
	int currentDownloadFileSize;

	QByteArray previousReadyReadData;
	QByteArray dataToSend;

	QString username;
	QString password;
};