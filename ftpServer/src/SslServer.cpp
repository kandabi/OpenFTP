#include "./headers/stdafx.h"
#include "./headers/SslServer.h"

SslServer::SslServer(QObject* parent) : QTcpServer(parent),
m_sslLocalCertificate(),
m_sslPrivateKey(),
m_sslProtocol(QSsl::UnknownProtocol)
{
}


void SslServer::incomingConnection(qintptr socketDescriptor)
{
    // Create socket
    QSslSocket* sslSocket = new QSslSocket(this);
    sslSocket->setSocketDescriptor(socketDescriptor);
    sslSocket->setLocalCertificate(m_sslLocalCertificate);
    sslSocket->setPrivateKey(m_sslPrivateKey);
    sslSocket->setProtocol(m_sslProtocol);
    sslSocket->startServerEncryption();

    // Add to the internal list of pending connections (see Qt doc: http://qt-project.org/doc/qt-5/qtcpserver.html#addPendingConnection)
    this->addPendingConnection(sslSocket);
}




const QSslCertificate& SslServer::getSslLocalCertificate() const
{
    return m_sslLocalCertificate;
}

const QSslKey& SslServer::getSslPrivateKey() const
{
    return m_sslPrivateKey;
}

QSsl::SslProtocol SslServer::getSslProtocol() const
{
    return m_sslProtocol;
}



void SslServer::setSslLocalCertificate(const QSslCertificate& certificate)
{
    m_sslLocalCertificate = certificate;
}

bool SslServer::setSslLocalCertificate(const QString& path, QSsl::EncodingFormat format)
{
    QFile certificateFile(path);

    if (!certificateFile.open(QIODevice::ReadOnly))
        return false;

    m_sslLocalCertificate = QSslCertificate(certificateFile.readAll(), format);
    QString str = m_sslLocalCertificate.toText();
    return true;
}


void SslServer::setSslPrivateKey(const QSslKey& key)
{
    m_sslPrivateKey = key;
}

bool SslServer::setSslPrivateKey(const QString& fileName, QSsl::KeyAlgorithm algorithm, QSsl::EncodingFormat format, const QByteArray& passPhrase)
{
    QFile keyFile(fileName);

    if (!keyFile.open(QIODevice::ReadOnly))
        return false;

    m_sslPrivateKey = QSslKey(keyFile.readAll(), algorithm, format, QSsl::PrivateKey, passPhrase);
    return true;
}


void SslServer::setSslProtocol(QSsl::SslProtocol protocol)
{
    m_sslProtocol = protocol;
}
