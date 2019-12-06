#pragma once
#include "stdafx.h"
#include "server_view.h"
#include "model.h"

class serverController : public QObject
{
	Q_OBJECT
public:
	serverController(int argc, char* argv[], QWidget* parent = Q_NULLPTR);
	int init();

private:
	void connectViewSignalSlots(QList<bool>& connectionResults);
	void connectModelSignalSlots(QList<bool>& connectionResults);

	QApplication app;
	serverView view;
	ServerModel data;
};

