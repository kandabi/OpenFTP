#include "stdafx.h"
#include "serialization_manager.h"

QString Serializer::ByteArrayToString(const QByteArray& data)
{
	return QString::fromStdString(data.toStdString());
}

QByteArray Serializer::JsonArrayToByteArray(const QJsonArray& json)
{
	QJsonDocument doc(json);
	return doc.toJson(QJsonDocument::Compact);
}

QByteArray Serializer::JsonObjectToByteArray(const QJsonObject& json)
{
	QJsonDocument doc(json);
	return doc.toJson(QJsonDocument::Compact);
}

QPixmap Serializer::decodePixmapFromString(const QString& icon) {
	auto const encoded = icon.toLatin1();
	QPixmap p;
	p.loadFromData(QByteArray::fromBase64(encoded), "PNG");
	return p;
}