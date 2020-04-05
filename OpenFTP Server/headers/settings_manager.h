#pragma once
#include "stdafx.h"
#include "user.h"
#include "encryption_manager.h"

class SettingsManager : QObject
{
	Q_OBJECT
public:
	 QMap<QUuid, User> getUsersFromSettings();
	 void writeUserToSettings(QString username, QString password, QString directoryPermission);
	 void removeUserFromSettings(int index);
	 QString getFtpDirectory();
	 int getPort();
	 void setPort(int port);
	 void setFtpDirectory(QString directory);
	 bool getFirstTimeTrayMessage();
	 void setFirstTimeTrayMessage();
	 bool getMinimizeToTray();
	 void setMinimizeToTray(const bool& minimize);

	SettingsManager(QObject* parent);

private:
	User getUser();
	QSettings* settings;
	const QString settingsDirectory = "./settings/server_settings.json";
	QSettings::Format jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

