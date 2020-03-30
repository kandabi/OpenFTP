#pragma once
#include "stdafx.h"
#include "user.h"

class Serializer 
{

public:
	static QString ByteArrayToString(const QByteArray& data);
	static QByteArray JsonToByteArray(const QJsonObject& data);
	static QByteArray JsonToByteArray(const QJsonArray& json);
	static QJsonValue encodePixmapForJson(const QPixmap& p);
	static QPixmap decodePixmapFromString(const QString& icon);

private:
	Serializer();
};