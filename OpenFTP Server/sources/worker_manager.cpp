#include "stdafx.h"
#include "worker_manager.h"

WorkerThread::WorkerThread(const QString& filePath, QSslSocket* _socket) : socket(_socket)
{
    qFile.setFileName(filePath);
    size = qFile.size();
}

void WorkerThread::run() {
    
    QByteArray fileData;
    if (!qFile.open(QIODevice::ReadOnly) || !qFile.isReadable())
        return;

    while (writtenBytes < size)
    {
        loadFile(fileData);
        writtenBytes += packetSize;
        emit writeDataSignal(socket ,fileData);

        QThread::msleep(60);
    }
}

void WorkerThread::loadFile(QByteArray& fileData) {
    qFile.seek(writtenBytes);
    fileData = qFile.read(packetSize);
}