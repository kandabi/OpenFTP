#pragma once
#include "stdafx.h"
#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>

class SslServer : public QTcpServer
{   Q_OBJECT

public:
	SslServer(QObject* parent = 0);

signals:
	void writeTextSignal(QString text, QColor color = Qt::black);

private:
	void incomingConnection(qintptr socketDescriptor) override;
	void sslErrors(const QList<QSslError>& errors);

	QSslKey sslKey;
	QSslCertificate serverCertificate;
	QList<QSslCertificate> certificateAuthorty;
	QSslSocket* socket;

	friend class serverController;
};


