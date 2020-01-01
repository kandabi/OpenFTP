#pragma once
#include "stdafx.h"
#include "user.h"
#include "simplecrypt.h"

class SettingsManager : QObject
{
	Q_OBJECT
public:
	 QList<User> getUsersFromSettings();
	 void writeUserToSettings(QString username, QString password, QString directoryPermission);
	 void removeUserFromSettings(int index);
	 QString getFtpDirectory();
	 void setFtpDirectory(QString directory);
	 bool getFirstTimeTrayMessage();
	 void setFirstTimeTrayMessage();
	 bool getMinimizeToTray();
	 void setMinimizeToTray(const bool& minimize);

	SettingsManager(QObject* parent);

private:
	User getUser();
	SimpleCrypt crypto;
	QSettings* settings;
	const QString settingsDirectory = "./settings/server_settings.json";
	QSettings::Format jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

