#include "stdafx.h"
#include "model.h"

serverModel::serverModel(QWidget* parent) : QObject(parent), networkManager(registeredUsers, parent), settingsManager(parent), logger(parent, "settings/Log.txt")
{
	if (!QSslSocket::supportsSsl()) {
		QMessageBox::information(0, "OpenFTP Server",
			"Missing openssl dll files, please reinstall OpenFTP.");
		exit(EXIT_FAILURE);
	}

	QCoreApplication::setOrganizationName("OpenFTP");
	QCoreApplication::setOrganizationDomain("OpenFTP.com");
	QCoreApplication::setApplicationName("OpenFTP");

	registeredUsers = settingsManager.getUsersFromSettings();
}

void serverModel::init()
{
	emit writeTextSignal("OpenFTP Server - " + (QString)APP_VERSION + ", written by kandabi");
	emit writeTextSignal("OpenFTP is an open source file transfer server and client, check it out on <a style='color: red;' href='https://github.com/kandabi/OpenFTP'>Github!</a> ");
	int serverPort = settingsManager.getPort();
	if (serverPort)
		emit setPortSignal(serverPort);
}


void serverModel::startServer(int port)
{
	emit writeTextSignal("Setting up server.");

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
	emit writeTextSignal("Closing server, disconnecting all users.");
	networkManager.stopServer();
}


void serverModel::saveFtpDirectory(QString directory)
{ 
	settingsManager.setFtpDirectory(directory);
	emit writeTextSignal("Set up FTP directory in: " + directory);
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(), getUserNamesFromUserMap(registeredUsers), settingsManager.getMinimizeToTray());
}


void serverModel::createUser(QString username, QString password, QString directoryPermitted)
{
	if (directoryPermitted.isEmpty())
		directoryPermitted = settingsManager.getFtpDirectory();

	settingsManager.writeUserToSettings(username, password, directoryPermitted);
	registeredUsers = settingsManager.getUsersFromSettings();
	emit initializeSettingsSignal(settingsManager.getFtpDirectory() , getUserNamesFromUserMap(registeredUsers), settingsManager.getMinimizeToTray());
}

void serverModel::ForceUserDisconnect(QString userName)
{
	networkManager.ForceUserDisconnect(userName);
}


void serverModel::deleteUser(int item)
{
	settingsManager.removeUserFromSettings(item);
	registeredUsers = settingsManager.getUsersFromSettings();
}


void serverModel::writeUsersToSettingsScreen()
{
	emit initializeSettingsSignal(settingsManager.getFtpDirectory(), getUserNamesFromUserMap(registeredUsers), settingsManager.getMinimizeToTray());
}


void serverModel::setMinimizeToTray(bool checked)
{
	settingsManager.setMinimizeToTray(checked);
}


QStringList serverModel::getUserNamesFromUserMap(const QMap<QUuid ,User>& userMap)
{
	QStringList nameList;
	for (auto user : userMap)
	{
		nameList.append(user.username);
	}
	return nameList;
}