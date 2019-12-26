#pragma once
#include "stdafx.h"
#include "request_manager.h"

struct connectionCredentials
{
	connectionCredentials::connectionCredentials(bool _checkboxChecked,QString _serverAddress, QString _serverPort, QString _userName, QString _userPassword) : 
		checkboxChecked(_checkboxChecked), serverAddress(_serverAddress), serverPort(_serverPort), userName(_userName), userPassword(_userPassword)
	{};

	bool checkboxChecked;
	QString serverAddress;
	QString serverPort;
	QString userName;
	QString userPassword;
};


class SettingsManager : QObject
{
	Q_OBJECT
public:
	SettingsManager(QObject* parent);

	QString getDefaultBrowserDirectory();
	void setDefaultBrowserDirectory(QString directory);
	RequestManager::FileOverwrite getOverwriteExistingFileBehavior();
	void setOverwriteExistingFileBehavior(const int& selection);
	connectionCredentials SettingsManager::getConnectionCredentials();
	void setConnectionCredentials(const bool& checkboxChecked ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	bool getShowTrayMessage();
	void setShowTrayMessage();
	bool getMinimizeToTray();
	void setMinimizeToTray(const bool& minimize);

private:
	QSettings* settings;
	const QString settingsDirectory = "./settings/client_settings.json";
	QSettings::Format jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

