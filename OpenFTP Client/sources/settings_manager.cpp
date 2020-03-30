#include "stdafx.h"
#include "settings_manager.h"

SettingsManager::SettingsManager(QObject* parent) : QObject(parent)
{
	settings = new QSettings(settingsDirectory, jsonFormat);
	crypto.setKey(CRYPTO_KEY);
}

QString SettingsManager::getDefaultBrowserDirectory()
{
	return settings->value("defaultLocalDirectory").toString();
}


bool SettingsManager::getMinimizeToTray()
{
	QVariant minimize = settings->value("minimizeToTray");

	if (!minimize.isValid())
	{
		setMinimizeToTray(true);
		return true;
	}
	return minimize.toBool();
}

void SettingsManager::setMinimizeToTray(const bool& minimize)
{
	settings->setValue("minimizeToTray", minimize);
}

RequestManager::FileOverwrite SettingsManager::getOverwriteExistingFileBehavior()
{
	return static_cast<RequestManager::FileOverwrite>(settings->value("overwriteExistingFileBehavior").toInt());
}


void SettingsManager::setConnectionCredentials(const bool& checkboxChecked ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword)
{
	settings->setValue("saveCredentials", checkboxChecked);
	settings->setValue("serverAddress", serverAddress);
	settings->setValue("serverPort", serverPort);
	settings->setValue("userName", crypto.encryptToString(userName));
	settings->setValue("userPassword", crypto.encryptToString(userPassword));
}


connectionCredentials SettingsManager::getConnectionCredentials()
{
	return connectionCredentials(settings->value("saveCredentials").toBool() ,settings->value("serverAddress").toString(), settings->value("serverPort").toString(), crypto.decryptToString(settings->value("userName").toString()), crypto.decryptToString(settings->value("userPassword").toString()));
}


bool SettingsManager::getShowTrayMessage()
{
	return settings->value("showTrayMessage").toBool();
}

void SettingsManager::setShowTrayMessage()
{
	settings->setValue("showTrayMessage", true);
}


void SettingsManager::setOverwriteExistingFileBehavior(const int& selection)
{
	settings->setValue("overwriteExistingFileBehavior", selection);
}


void SettingsManager::setDefaultBrowserDirectory(QString directory)
{
	 settings->setValue("defaultLocalDirectory", directory);
}


void SettingsManager::setAppStyle(QString file)
{
	settings->setValue("appStyle", file);
}


QString SettingsManager::getAppStyle()
{
	QVariant style = settings->value("appStyle");
	if (!style.isValid())
	{
		setAppStyle("default_style.css");
		return "default_style.css";
	}
	return style.toString();
}


bool SettingsManager::readJsonFile(QIODevice& device, QSettings::SettingsMap& map)
{
	QJsonDocument json = QJsonDocument::fromJson(device.readAll());
	map = json.object().toVariantMap();
	return true;
}

bool SettingsManager::writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map)
{
	device.write(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson());
	return true;
}