#include "stdafx.h"
#include "ssl_server.h"

SslServer::SslServer(QObject* parent) : QTcpServer(parent)
{
	QFile keyFile((QString)CERTIFICATES_DIR + "server_private.key");
	keyFile.open(QIODevice::ReadOnly);
	sslKey = QSslKey(keyFile.readAll(), QSsl::Rsa);
	keyFile.close();

	serverCertificate = QSslCertificate::fromPath((QString)CERTIFICATES_DIR + "server_certificate.crt").first();
	certificateAuthorty = QSslCertificate::fromPath((QString)CERTIFICATES_DIR + "root_certificate.crt");
}


void SslServer::incomingConnection(qintptr socketDescriptor)
{
	socket = new QSslSocket(this);
	
	if (socket->setSocketDescriptor(socketDescriptor)) 
	{
		connect(socket, qOverload<const QList<QSslError>&>(&QSslSocket::sslErrors), this, &SslServer::sslErrors);
		socket->setPrivateKey(sslKey);
		socket->setLocalCertificate(serverCertificate);
		socket->addCaCertificates(certificateAuthorty);
		socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
		socket->startServerEncryption();

		addPendingConnection(socket);
	}
	else {
		socket->deleteLater();
	}
}


void SslServer::sslErrors(const QList<QSslError>& errors)
{
	foreach(const QSslError &error, errors)
		emit writeTextSignal(error.errorString(), Qt::darkRed);
}