#pragma once
#include "stdafx.h"
#include "ui_serverView.h"
#include "settings_view.h"
#include "settings_manager.h"

class serverView : public QMainWindow
{
	Q_OBJECT

public:
	serverView(QWidget *parent = Q_NULLPTR);

	QMenu menu;
	QMenu trayIconMenu;
signals:
	void saveSettingsSignal();
	void ForceUserDisconnectSignal(QString userName);
	void deleteUserSignal(QString username);
	void initServerSignal(int port);

public slots:
	void writeTextToScreen(QString text, QColor color);
	void openSettingsMenu();
	void closeSettingsMenu();
	void startServer();
	void stopServer();
	void ForceUserDisconnect();
	void connectUserToList(QString name);
	void deleteUserFromList(QString name);
	void showContextMenu(const QPoint& pos);
	void closeEvent(QCloseEvent* event) override;
	void closeWindow();
	void initServer();
	void activateTrayIcon(QSystemTrayIcon::ActivationReason reason);
	void setPort(int port);

private:
	bool closing = false;
	bool serverIsRunning = false;

	
	QSystemTrayIcon systemTrayIcon;
	QIcon icon;
	SettingsManager settingsManager;

	Ui::serverGui ui;
	settingsView settingsView;
	friend class serverController;
};
