#include "./headers/stdafx.h"
#include "./headers/settings_manager.h"

SettingsManager::SettingsManager(QObject* parent) : QObject(parent)
{
	settings = new QSettings(settingsDirectory, jsonFormat);
	//settings = new QSettings(jsonFormat, QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName(), parent);
}

void SettingsManager::writeUserToSettings(QString username, QString password, QString directoryPermission)
{
	//**** Needs validation.

	int initialArraySize = settings->value("user/size").toInt();

	settings->beginWriteArray("user");

	settings->setArrayIndex(initialArraySize);
	settings->setValue("name", username);
	settings->setValue("password", password);
	settings->setValue("directory", directoryPermission);
	//settings->setValue("write", true);
	//settings->setValue("delete", true);

	settings->sync();
	settings->endArray();
}

QString SettingsManager::getFtpDirectory()
{
	return settings->value("mainDirectory").toString();
}

void SettingsManager::setFtpDirectory(QString directory)
{
	 settings->setValue("mainDirectory", directory);
}




QList<User> SettingsManager::getUsersFromSettings()
{
	QList<User> userList;
	int arraySize = settings->beginReadArray("user");

	for (int i = 0; i < arraySize; ++i) {
		settings->setArrayIndex(i);
		userList.append(User{ settings->value("name").toString(), settings->value("password").toString() , settings->value("directory").toString() });
	}
	settings->endArray();

	return userList;
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

void SettingsManager::removeUserFromSettings(QString username)
{
	//*** Implementation Forthcoming
}