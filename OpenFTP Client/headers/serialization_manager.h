#pragma once
#include "stdafx.h"
#include "user.h"

class Serializer
{
public:
	static QString ByteArrayToString(const QByteArray& data);
	static QByteArray JsonArrayToByteArray(const QJsonArray& json);
	static QByteArray JsonObjectToByteArray(const QJsonObject& json);
	static QPixmap Serializer::decodePixmapFromString(const QString& icon);

private:
	Serializer();
};