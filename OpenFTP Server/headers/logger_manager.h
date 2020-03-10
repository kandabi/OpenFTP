#pragma once
#include "stdafx.h"

class LoggerManager : public QObject
{
    Q_OBJECT
public:
    LoggerManager(QObject* parent, QString fileName);
    ~LoggerManager();

private:
    QFile* file;

signals:

public slots:
    void logToFile(const QString& value);

};
