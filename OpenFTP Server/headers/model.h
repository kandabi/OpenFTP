#pragma once
#include "stdafx.h"
#include "network_manager.h"
#include "settings_manager.h"
#include "logger_manager.h"

class serverModel : public QObject 
{   
	Q_OBJECT 
    friend class serverController;
public:
	serverModel(QWidget* parent = Q_NULLPTR);
	void init();

signals:
	void writeTextSignal(QString text, QColor color = Qt::black);
	void initializeSettingsSignal(QString directory, QStringList nameList, bool minimizeToTray);
	void startServerSignal();
	void stopServerSignal();
	void connectUserToListSignal(QString text);
	void deleteUserFromListSignal(QString text);
	void setPortSignal(int port);

public slots:
	void startServer(int port);
	void stopServer();

	void writeUsersToSettingsScreen();
	void deleteUser(int item);
	void saveFtpDirectory(QString directory);
	void createUser(QString username, QString password, QString directoryPermitted);
	void ForceUserDisconnect(QString userName);
	void setMinimizeToTray(bool checked);

private:
	SettingsManager settingsManager;
	NetworkManager networkManager;
	LoggerManager logger;
	QList<User> registeredUsersList;

	QStringList getUserNamesFromUserList(const QList<User>& userList);
};

