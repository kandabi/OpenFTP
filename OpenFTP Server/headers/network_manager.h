#pragma once
#include "stdafx.h"
#include "ssl_server.h"
#include "serialization_manager.h"
#include "ftp_manager.h"
#include "transfer.h"

class NetworkManager : public QObject
{
	Q_OBJECT
public:
	NetworkManager(QMap<QUuid ,User>& _registeredUsersList, QWidget* parent = Q_NULLPTR);

	bool initServer(const int& port);
	bool stopServer();
	void ForceUserDisconnect(QString userName);

public slots:
	void newConnectionAttempt();
	void onReadyRead();
	void onSocketStateChanged(QAbstractSocket::SocketState socketState);

signals:
	void writeTextSignal(QString text, QColor color = Qt::black);
	void connectUserToListSignal(QString text);
	void deleteUserFromListSignal(QString text);

private:
	QUuid validateUser(const QMap<QUuid, User>& userList, QString name, QString password);
	bool writeToClient(QSslSocket* socketToWrite, const QByteArray& data);
	QUuid getUserGuidBySocket(QSslSocket* socket);
	QSslSocket* getCurrentSocket();
	void parseJson(const QByteArray& data, QSslSocket* socket, const QUuid& userGuid);
	void parseUpload(const QByteArray& data, QSslSocket* socket, const QUuid& userGuid);
	bool disconnectUserSocket(QSslSocket* socket);
	//int getTransferByUserGuid(const QUuid& userGuid);
	void isEncrypted(const QSslSocket* socket);

	QMap<QUuid ,User>& registeredUsers;
	QMap<QUuid ,User> connectedUsers;
	QMap<QUuid ,Transfer> transfersInProgress;
	//QTcpServer server;
	SslServer server;

	friend class serverController;
};