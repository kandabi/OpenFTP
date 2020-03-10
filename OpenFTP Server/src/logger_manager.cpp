#include "stdafx.h"
#include "logger_manager.h"

LoggerManager::LoggerManager(QObject* parent, QString fileName) : QObject(parent) {
    if (!fileName.isEmpty()) {
        file = new QFile;
        file->setFileName(fileName);
        file->open(QIODevice::Append | QIODevice::Text);
    }
}

void LoggerManager::logToFile(const QString& value) {
    QString text = "[" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + "] - " +  value + "\n";
    QTextStream out(file);
    out.setCodec("UTF-8");
    if (file != 0) {
        out << text;
    }
}

LoggerManager::~LoggerManager() {
    if (file != 0)
        file->close();
}