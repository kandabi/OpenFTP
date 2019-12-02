#include "./headers/stdafx.h"
#include "./headers/user.h"

User::User()
{}

User::User(QString _username, QString _password) :
	username(_username), password(_password)
{
}