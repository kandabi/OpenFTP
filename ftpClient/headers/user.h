#pragma once
#include "stdafx.h"

class User
{

public:
	User() {};
	User(QString _username, QString _password) :
		username(_username), password(_password) {};

	QString username;
	QString password;

};