#pragma once
#include "stdafx.h"

class WorkerThread : public QThread 
{   
    Q_OBJECT
    void run() override {
        Q_ASSERT(qFile.open(QIODevice::ReadOnly));
        QByteArray fileData = qFile.readAll();
        emit setUploadDataToSendSignal(fileData);
        emit sendRequestData(requestData);
    }

    QFile qFile;
    QByteArray requestData;

signals:
    void setUploadDataToSendSignal(const QByteArray& fileData);
    void sendRequestData(const QByteArray& requestData);

public:
    WorkerThread::WorkerThread(const QString& filePath, const QByteArray& _requestData)
    {
       qFile.setFileName(filePath);
       requestData = _requestData;
    }
};