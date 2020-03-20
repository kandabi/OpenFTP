#pragma once
#include "stdafx.h"
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
	void disconnectUser(QString userName);

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
	bool writeToClient(QTcpSocket* socketToWrite, const QByteArray& data);
	int getUserIndexBySocket(QTcpSocket* socket);
	QTcpSocket* getCurrentSocket();
	void parseJson(const QByteArray& data, QTcpSocket* socket, int userIndex);
	void parseUpload(const QByteArray& data, QTcpSocket* socket, int userIndex);
	bool disconnectUserSocket(QTcpSocket* socket);
	int getTransferByUserIndex(const int& userIndex);

	QList<User>& registeredUsersList;
	QList<User> connectedUsers;
	QList<Transfer> transfersInProgress;
	QTcpServer server;
	//SslServer server;

	friend class serverController;
};