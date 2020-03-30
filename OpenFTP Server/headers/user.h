#pragma once
#include "stdafx.h"

class User
{
private:
	QSslSocket* socket = Q_NULLPTR;

public:
	QString username;
	QString password;
	QString homeDirectory;
	
	bool transferInProgress = false;

	User() {};

	User(QString _username, QString _password, QString _directory = "") :
		username(_username), password(_password), homeDirectory(_directory) {};

	inline void setSocket(QSslSocket* _socket)
	{ 
		socket = _socket; 
	}

	inline QSslSocket* getSocket()
	{
		if (socket != Q_NULLPTR)
			return socket;
		else
			return Q_NULLPTR;
	}

	inline qintptr getSocketDescriptor()
	{
		return socket->socketDescriptor();
	}
};