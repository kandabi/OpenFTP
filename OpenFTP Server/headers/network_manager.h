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
	NetworkManager(QList<User>& _registeredUsersList, QWidget* parent = Q_NULLPTR);

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
	int validateUser(const QList<User>& userList, QString name, QString password);
	bool writeToClient(QSslSocket* socketToWrite, const QByteArray& data);
	int getUserIndexBySocket(QSslSocket* socket);
	QSslSocket* getCurrentSocket();
	void parseJson(const QByteArray& data, QSslSocket* socket, int userIndex);
	void parseUpload(const QByteArray& data, QSslSocket* socket, int userIndex);
	bool disconnectUserSocket(QSslSocket* socket);
	int getTransferByUserIndex(const int& userIndex);
	void isEncrypted(const QSslSocket* socket);

	QList<User>& registeredUsersList;
	QList<User> connectedUsers;
	QList<Transfer> transfersInProgress;
	//QTcpServer server;
	SslServer server;

	friend class serverController;
};