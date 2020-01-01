#pragma once
#include "stdafx.h"
#include "ui_settingsView.h"

class settingsView : public QDialog
{
	Q_OBJECT

public:
	settingsView(QWidget *parent = Q_NULLPTR);
	~settingsView();
	QMenu menu;
	
signals:
	void onSave();
	void registerUserSignal(QString username, QString password, QString directoryPermitted = "");
	void deleteUserSignal(int row);
	void setFtpDirectorySignal(QString directory);

public slots:
	void initializeSettings(QString directory, QStringList nameList, bool minimizeToTray);
	void selectMainFtpDirectory();
	void selectUserFtpDirectory();
	void showContextMenu(const QPoint&);
	void deleteUser();
	void saveSettings();
	void applySettings();
	void closeSettingsMenu();

private slots:
	void registerUser();

private:
	bool applyButtonClicked = false;

	Ui::settingsGui ui;
	friend class serverView;
	friend class serverController;
};


