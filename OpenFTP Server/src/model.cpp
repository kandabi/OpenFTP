#include "stdafx.h"
#include "model.h"


serverModel::serverModel(QWidget* parent) : QObject(parent), networkManager(registeredUsersList, parent), settingsManager(parent), logger(parent, "Log.txt")
{
	QCoreApplication::setOrganizationName("OpenFTP");
	QCoreApplication::setOrganizationDomain("OpenFTP.com");
	QCoreApplication::setApplicationName("OpenFTP");

	//auto appPath = QCoreApplication::applicationDirPath();
	//auto libraryPaths = QCoreApplication::libraryPaths();
	//libraryPaths.append(appPath + "/plugins");
	//libraryPaths.append(appPath + "/lib");
	//QCoreApplication::setLibraryPaths(libraryPaths);

	registeredUsersList = settingsManager.getUsersFromSettings();
}

void serverModel::init()
{
	emit writeTextSignal("OpenFTP Server 0.2.6, written by kandabi", Qt::darkGray);
	emit writeTextSignal("OpenFTP is an open source file transfer server and client, check it out on <a style='color: red;' href='https://github.com/kandabi/OpenFTP'>Github!</a> ", Qt::darkGray);
	int serverPort = settingsManager.getPort();
	if (serverPort)
		emit setPortSignal(serverPort);
}


void serverModel::startServer(int port)
{
	emit writeTextSignal("Setting up OpenFTP Server.");

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
	if (networkManager.initServer(port))
	{
		settingsManager.setPort(port);
	}
}


void serverModel::stopServer()
{
	emit stopServerSignal();
	emit writeTextSignal("Closing OpenFTP Server, disconnecting all users.");
	networkManager.stopServer();
}


void serverModel::saveFtpDirectory(QString directory)
{ 
	settingsManager.setFtpDirectory(directory);
	emit writeTextSignal("Set up FTP directory in: " + directory);
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(), getUserNamesFromUserList(registeredUsersList), settingsManager.getMinimizeToTray());
}


void serverModel::createUser(QString username, QString password, QString directoryPermitted)
{
	if (directoryPermitted.isEmpty())
		directoryPermitted = settingsManager.getFtpDirectory();

	settingsManager.writeUserToSettings(username, password, directoryPermitted);
	registeredUsersList = settingsManager.getUsersFromSettings();
	emit initializeSettingsSignal(settingsManager.getFtpDirectory() ,getUserNamesFromUserList(registeredUsersList), settingsManager.getMinimizeToTray());
}

void serverModel::disconnectUser(QString userName)
{
	networkManager.disconnectUser(userName);
}


void serverModel::deleteUser(int item)
{
	settingsManager.removeUserFromSettings(item);
}


void serverModel::writeUsersToSettingsScreen()
{
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(),getUserNamesFromUserList(registeredUsersList), settingsManager.getMinimizeToTray());
}


void serverModel::setMinimizeToTray(bool checked)
{
	settingsManager.setMinimizeToTray(checked);
}


QStringList serverModel::getUserNamesFromUserList(const QList<User>& userList)
{
	QStringList nameList;
	for (auto user : userList)
	{
		nameList.append(user.username);
	}
	return nameList;
}


//void serverModel::saveSettings() //*** Unused Function
//{
//	emit writeTextSignal("Settings have been saved. (Not implemented yet.) ", Qt::red);
//	emit closeSettingsSignal();
//}

