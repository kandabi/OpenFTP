#pragma once
#include "stdafx.h"

class WorkerThread : public QThread 
{   
    Q_OBJECT
public:
    static const qint64 packetSize = 300000000; //*** Files will be split to 300 Mb
    static const qint64 filesizeToSplit = 500000000;
    WorkerThread::WorkerThread(const QString& filePath, const quint64 _readFromPosition);

signals:
    void sendRequestData(const QByteArray& requestData);

private:
    void run() override;
    void WorkerThread::loadFile(QByteArray& fileData);

    quint64 size;
    QFile qFile;
    quint64 readFromPosition = packetSize;
};