#include "./headers/stdafx.h"
#include "./headers/model.h"


ServerModel::ServerModel(QWidget* parent) : QObject(parent), networkManager(registeredUsersList, parent), settingsManager(parent)
{
	//QCoreApplication::setOrganizationName("kandabi");
	//QCoreApplication::setOrganizationDomain("kandabi.com");
	//QCoreApplication::setApplicationName("qtFtpServer");

	registeredUsersList = settingsManager.getUsersFromSettings();
}


void ServerModel::initServer()
{
	emit writeTextSignal("Setting up FTP Server");

	QString ftpDirectory = settingsManager.getFtpDirectory();
	QDir dir;
	if (ftpDirectory.isEmpty())
	{
		emit writeTextSignal("An FTP directory has not yet been set, please go to the Tools -> Settings menu and choose a directory.", Qt::darkRed);
		return;
	}
	else if (!dir.exists(ftpDirectory))
	{
		dir.mkdir(ftpDirectory);
		emit writeTextSignal("FTP directory created in: " + ftpDirectory);
	}

	emit startServerSignal();
	networkManager.initServer();
}


void ServerModel::stopServer()
{
	emit stopServerSignal();
	networkManager.stopServer();
}


void ServerModel::saveSettings()
{
	emit writeTextSignal("Settings have been saved. (Not implemented yet.) ", Qt::red);
	emit closeSettingsSignal();
}


void ServerModel::saveFtpDirectory(QString directory)
{ 
	settingsManager.setFtpDirectory(directory);
	emit writeTextSignal("Set up FTP directory in: " + directory);
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(), getUserNamesFromUserList(registeredUsersList));
}



void ServerModel::createUser(QString username, QString password, QString directoryPermitted)
{
	//**** Needs validation.
	if (directoryPermitted.isEmpty())
		directoryPermitted = settingsManager.getFtpDirectory();

	settingsManager.writeUserToSettings(username, password, directoryPermitted);
	registeredUsersList = settingsManager.getUsersFromSettings();
	emit initializeSettingsSignal(settingsManager.getFtpDirectory() ,getUserNamesFromUserList(registeredUsersList));
}

void ServerModel::disconnectUser(QString userName)
{
	networkManager.disconnectUser(userName);
}


void ServerModel::deleteUser(int item)
{
	//*** Implementation Forthcoming
	settingsManager.removeUserFromSettings("");
}


void ServerModel::writeUsersToSettingsScreen()
{
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(),getUserNamesFromUserList(registeredUsersList));
}


QStringList ServerModel::getUserNamesFromUserList(const QList<User>& userList)
{
	QStringList nameList;
	for (auto user : userList)
	{
		nameList.append(user.username);
	}
	return nameList;
}

