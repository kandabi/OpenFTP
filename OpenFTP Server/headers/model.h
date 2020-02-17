#pragma once
#include "stdafx.h"
#include "network_manager.h"
#include "settings_manager.h"



class serverModel : public QObject 
{   
	Q_OBJECT 
    friend class serverController;
public:
	serverModel(QWidget* parent = Q_NULLPTR);

signals:
	void writeTextSignal(QString text, QColor color = Qt::black);
	void initializeSettingsSignal(QString directory, QStringList nameList, bool minimizeToTray);
	void closeSettingsSignal();
	void startServerSignal();
	void stopServerSignal();
	void connectUserToListSignal(QString text);
	void deleteUserFromListSignal(QString text);

public slots:
	void initServer(int port);
	void stopServer();

	void saveSettings();
	void writeUsersToSettingsScreen();
	void deleteUser(int item);
	void saveFtpDirectory(QString directory);
	void createUser(QString username, QString password, QString directoryPermitted);
	void disconnectUser(QString userName);
	void setMinimizeToTray(bool checked);

private:
	SettingsManager settingsManager;
	NetworkManager networkManager;
	QList<User> registeredUsersList;

	QStringList getUserNamesFromUserList(const QList<User>& userList);
};

