#pragma once
#include "stdafx.h"
#include "request_manager.h"
#include "simplecrypt.h"

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

	void setDefaultBrowserDirectory(QString directory);
	void setOverwriteExistingFileBehavior(const int& selection);
	void setConnectionCredentials(const bool& checkboxChecked, const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	void setShowTrayMessage();
	void setMinimizeToTray(const bool& minimize);
	void setAppStyle(QString file);

	QString getDefaultBrowserDirectory();
	bool getShowTrayMessage();
	bool getMinimizeToTray();
	QString getAppStyle();
	RequestManager::FileOverwrite getOverwriteExistingFileBehavior();
	connectionCredentials getConnectionCredentials();

private:
	QSettings* settings;
	SimpleCrypt crypto;
	const QString settingsDirectory = "./settings/client_settings.json";
	QSettings::Format jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

	static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
	static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

