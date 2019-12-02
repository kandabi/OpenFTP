#pragma once
#include "stdafx.h"

class User
{
private:
	QTcpSocket* socket;

public:
	QString username;
	QString password;
	QString homeDirectory;
	bool transferInProgress = false;

	User();
	User(QString _username, QString _password, QString directory = "");

	inline void setSocket(QTcpSocket* _socket)
	{ 
		socket = _socket; 
	}

	inline QTcpSocket* getSocket()
	{
		if (socket != Q_NULLPTR)
			return socket;
		else
			return Q_NULLPTR;
	}
};