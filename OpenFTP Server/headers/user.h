#pragma once
#include "stdafx.h"

class User
{
private:
	QSslSocket* socket = Q_NULLPTR;
	QUuid guid;

public:
	QString username;
	QString password;
	QString homeDirectory;
	
	bool transferInProgress = false;

	User() {};

	User(QUuid _guid, QString _username, QString _password, QString _directory = "") :
		guid(_guid),username(_username), password(_password), homeDirectory(_directory) {};

	inline void setSocket(QSslSocket* _socket)
	{ 
		socket = _socket; 
	}

	inline QUuid getGuid() const
	{
		return guid;
	}

	inline QSslSocket* getSocket() const
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