#include "./headers/stdafx.h"
#include "./headers/serialization_manager.h"


QByteArray Serializer::JsonToByteArray(const QJsonObject& jsonObject)
{
	QJsonDocument doc(jsonObject);
	return doc.toJson();
}

QByteArray Serializer::JsonToByteArray(const QJsonArray& json)
{
	QJsonDocument doc(json);
	return doc.toJson();
}

QString Serializer::ByteArrayToString(const QByteArray& data)
{
	return QString::fromStdString(data.toStdString());
}

QPixmap Serializer::decodePixmapFromString(const QString& icon) {
	auto const encoded = icon.toLatin1();
	QPixmap p;
	p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
	return p;
}