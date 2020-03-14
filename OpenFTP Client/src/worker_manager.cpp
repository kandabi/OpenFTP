#include "stdafx.h"
#include "worker_manager.h"

WorkerThread::WorkerThread(const QString& filePath, const quint64 _readFromPosition)
{
    qFile.setFileName(filePath);
    readFromPosition = _readFromPosition;
    size = qFile.size();
}

void WorkerThread::run() {
    
    QByteArray fileData;
    if (!qFile.open(QIODevice::ReadOnly) || !qFile.isReadable())
        return;

    loadFile(fileData);
    readFromPosition += packetSize;
    emit sendRequestData(fileData);
    //fileData.clear();
}

void WorkerThread::loadFile(QByteArray& fileData) {
    qFile.seek(readFromPosition);
    fileData = qFile.read(packetSize);
}