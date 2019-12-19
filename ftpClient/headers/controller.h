#pragma once
#include "stdafx.h"
#include "client_view.h"
#include "model.h"

class clientController : public QObject
{

public:
	clientController(int argc, char* argv[], QWidget* parent = Q_NULLPTR);
	int init();

private:
	void connectViewSignalSlots(QList<bool>&connectionResults);
	void connectModelSignalSlots(QList<bool>& connectionResults);

	Q_OBJECT
	QApplication app;
	clientView view;
	clientModel data;
};	

