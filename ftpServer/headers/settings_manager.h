#pragma once
#include "stdafx.h"
#include "user.h"

class SettingsManager : QObject
{
	Q_OBJECT
public:
	 QList<User> getUsersFromSettings();
	 void writeUserToSettings(QString username, QString password, QString directoryPermission);
	 void removeUserFromSettings(QString username);
	 QString getFtpDirectory();
	 void setFtpDirectory(QString directory);

	SettingsManager(QObject* parent);
	//~SettingsManager();

private:
	QSettings* settings;
	const QString settingsDirectory = "./settings/server_settings.json";
	QSettings::Format jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

