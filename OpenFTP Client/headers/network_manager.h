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

signals:
	void writeTextSignal(QString text, QColor color = {});
	void disconnectedFromServerSignal();
	void parseJsonSignal(const QByteArray& jsonArray);
	void updateProgressBarSignal(quint64 bytesReceived, quint64 bytesTotal);
	void setProgressBarSignal();
	void checkRemainingDownloadsSignal();


public slots:
	void setUploadDataToSend(const QByteArray& data);
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);
	void sslErrors(const QList<QSslError>& errors);
	void connectToServer(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	void disconnectFromServer();
	void uploadFileData();
	void continueLargeUpload();
	QByteArray parseByteData();
	void parseByteDownload(const QByteArray& data);
	void beginPendingDownload(const File& currentDownload, const QString& directoryToSave);
	void writeData(const QByteArray& data);
	void flushSocket();
	QByteArray readAll();
	void setdownloadInProgress(const bool& download);
	QString getSocketAddress();
	QString getSocketPort();
	void isEncrypted();

	inline bool isDownloading()
	{
		return downloadInProgress;
	}

public:
	bool splitUploadToChunks = false;
	int packetsSent = 0;
	QFile qFile;
	static const qint64 packetSize = 6000000; //*** Large Files will be split to 6 Mb packets
	static const qint64 filesizeToSplit = 200000000;

private:
	//QTcpSocket socket;
	QSslSocket socket;
	QList<QSslError> expectedSslErrors;
	QSaveFile qSaveFile;

	bool downloadInProgress = false;

	int packetsRecieved = 0;
	quint64 writtenBytes = 0;
	quint64 currentDownloadFileSize;

	QByteArray previousReadyReadData;
	QByteArray dataToSend;

	QString username;
	QString password;
};