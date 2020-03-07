#pragma once
#include "stdafx.h"

class WorkerThread : public QThread 
{   
    Q_OBJECT
    void run() override {

        QByteArray fileData;
        quint64 size = qFile.size();

        if (!qFile.open(QIODevice::ReadOnly) || !qFile.isReadable())
            return;

        while (readFromPosition < size)
        {
            loadFile(fileData);
            readFromPosition += filesizeToSplit;

            emit setUploadDataToSendSignal(fileData);

            if (!initialRequestDataSent)
            {
                emit sendRequestData(requestData);
                initialRequestDataSent = true;
            }
            else {
                emit sendRequestData(fileData);
            }
        }
    }


    void loadFile(QByteArray& fileData) {
        qFile.seek(readFromPosition);
        fileData = qFile.read(filesizeToSplit);
    }


    bool initialRequestDataSent = false;
    QFile qFile;
    QByteArray requestData;
    quint64 readFromPosition = filesizeToSplit;

signals:
    void setUploadDataToSendSignal(const QByteArray& fileData);
    void sendRequestData(const QByteArray& requestData);

public:
    static const qint64 filesizeToSplit = 300000000; //*** Files will be split to 300 Mb

    WorkerThread::WorkerThread(const QString& filePath, const quint64 _readFromPosition, QByteArray _requestData)
    {
       qFile.setFileName(filePath);
       requestData = _requestData;
       readFromPosition = _readFromPosition;
    }
};