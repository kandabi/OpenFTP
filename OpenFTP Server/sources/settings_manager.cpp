#include "stdafx.h"
#include "settings_manager.h"


SettingsManager::SettingsManager(QObject* parent) : QObject(parent)
{
	settings = new QSettings(settingsDirectory, jsonFormat);

	//encryptionManager crypto;
	////crypto.setKey(CRYPTO_KEY);
	//QString encryptedStr = crypto.encrypt("testing...");
	//QString decryptedStr = crypto.decrypt(encryptedStr);
}

void SettingsManager::writeUserToSettings(QString username, QString password, QString directoryPermission)
{
	int initialArraySize = settings->value("user/size").toInt();

	settings->beginWriteArray("user");

	settings->setArrayIndex(initialArraySize);

	settings->setValue("guid", QUuid::createUuid());
	settings->setValue("name", username);
	settings->setValue("password", password);
	settings->setValue("directory", directoryPermission);

	settings->sync();
	settings->endArray();
}

QString SettingsManager::getFtpDirectory()
{
	return settings->value("mainDirectory").toString();
}

int SettingsManager::getPort()
{
	return settings->value("serverPort").toInt();
}

void SettingsManager::setPort(int port)
{
	settings->setValue("serverPort", port);
}

void SettingsManager::setFtpDirectory(QString directory)
{
	 settings->setValue("mainDirectory", directory);
}

bool SettingsManager::getFirstTimeTrayMessage()
{
	return settings->value("firstTimeTrayMessage").toBool();
}

void SettingsManager::setFirstTimeTrayMessage()
{
	settings->setValue("firstTimeTrayMessage", true);
}

bool SettingsManager::getMinimizeToTray()
{
	if (!settings->value("minimizeToTray").isValid())
	{
		setMinimizeToTray(true);
		return true;
	}
	return settings->value("minimizeToTray").toBool();
}

void SettingsManager::setMinimizeToTray(const bool& minimize)
{
	settings->setValue("minimizeToTray", minimize);
}


QMap<QUuid ,User> SettingsManager::getUsersFromSettings()
{
	QMap<QUuid, User> userMap;
	int arraySize = settings->beginReadArray("user");

	for (int i = 0; i < arraySize; ++i) {
		settings->setArrayIndex(i);

		User user = getUser();
		userMap.insert(user.getGuid(), user);
	}

	settings->endArray();
	return userMap;
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

void SettingsManager::removeUserFromSettings(int index)
{
	QList<User> userList;
	int arraySize = settings->beginReadArray("user");

	for (int i = 0; i < arraySize; ++i) 
	{
		settings->setArrayIndex(i);

		if (i != index)
		{
			userList.append(getUser());
		}

		settings->remove("guid");
		settings->remove("name");
		settings->remove("password");
		settings->remove("directory");
	}

	settings->endArray();
	settings->remove("user/size");

	settings->sync();

	for (const User& user : userList)
		writeUserToSettings(user.username, user.password, user.homeDirectory);

}


User SettingsManager::getUser()
{
	return User {
		settings->value("guid").toString(),
		settings->value("name").toString(),
		settings->value("password").toString(),
		settings->value("directory").toString()
	};
}