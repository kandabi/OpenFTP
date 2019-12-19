#pragma once
#include "stdafx.h"
#include "ui_serverView.h"
#include "settings_view.h"

class serverView : public QMainWindow
{
	Q_OBJECT

public:
	serverView(QWidget *parent = Q_NULLPTR);

	QMenu menu;
signals:
	void saveSettingsSignal();
	void disconnectUserSignal(QString userName);
	void deleteUserSignal(QString username);

public slots:
	void writeTextToScreen(QString text, QColor color);
	void openSettingsMenu();
	void closeSettingsMenu();
	void startServer();
	void stopServer();
	void disconnectUser();
	void connectUserToList(QString name);
	void deleteUserFromList(QString name);
	void closeEvent(QCloseEvent* event);
	void showContextMenu(const QPoint& pos);


private:
	bool serverIsRunning = false;

	Ui::serverGui ui;
	settingsView settingsView;
	friend class serverController;
};
