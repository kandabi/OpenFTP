#pragma once
#include "stdafx.h"

class WorkerThread : public QThread 
{   
    Q_OBJECT
public:
    WorkerThread::WorkerThread(const QString& filePath, QTcpSocket* _socket);

    static const qint64 packetSize = 6000000; //*** Large Files will be split to 6 Mb chunks
    static const qint64 filesizeToSplit = 200000000;

signals:
    void writeDataSignal(QTcpSocket* socket, const QByteArray& data);

private:
    void run() override;
    void WorkerThread::loadFile(QByteArray& fileData);

    QTcpSocket* socket;
    QFile qFile;
    quint64 writtenBytes = 0;
    quint64 size = 0;
};