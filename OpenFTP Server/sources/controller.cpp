#include "stdafx.h"
#include "controller.h"

serverController::serverController(int argc, char* argv[], QWidget* parent) : QObject(parent), app(argc, argv)
{
	QList<bool> connectionResults;

	connectViewSignalSlots(connectionResults);
	connectModelSignalSlots(connectionResults);

	Q_ASSERT(!connectionResults.contains(false));
	Q_UNUSED(connectionResults);
}

void serverController::connectViewSignalSlots(QList<bool>& connectionResults)
{
	connectionResults.append(connect(view.ui.startServerButton, &QPushButton::clicked, &view, &serverView::initServer));
	connectionResults.append(connect(&view, &serverView::initServerSignal, &data, &serverModel::startServer));
	connectionResults.append(connect(view.ui.stopServerButton, &QPushButton::clicked, &data, &serverModel::stopServer));

	connectionResults.append(connect(view.ui.actionExit, &QAction::triggered, &view, &serverView::closeWindow));
	connectionResults.append(connect(&view.systemTrayIcon, &QSystemTrayIcon::activated, &view, &serverView::activateTrayIcon));
	connectionResults.append(connect(view.settingsView.ui.minimizeToTray, &QCheckBox::clicked, &data, &serverModel::setMinimizeToTray));

	connectionResults.append(connect(view.ui.actionSettings, &QAction::triggered, &data, &serverModel::writeUsersToSettingsScreen));
	connectionResults.append(connect(view.ui.actionSettings, &QAction::triggered, &view, &serverView::openSettingsMenu));
	connectionResults.append(connect(view.settingsView.ui.registerUserButton, &QPushButton::clicked, &view.settingsView, &settingsView::registerUser));
	connectionResults.append(connect(&view.settingsView, &settingsView::registerUserSignal, &data, &serverModel::createUser));
	connectionResults.append(connect(view.settingsView.ui.directorySelector, &QPushButton::clicked, &view.settingsView, &settingsView::selectMainFtpDirectory));
	connectionResults.append(connect(view.settingsView.ui.directoryPermittedSelector, &QPushButton::clicked, &view.settingsView, &settingsView::selectUserFtpDirectory));
	connectionResults.append(connect(&view.settingsView, &settingsView::setFtpDirectorySignal, &data, &serverModel::saveFtpDirectory));

	connectionResults.append(connect(view.settingsView.ui.userListWidget, &QListWidget::customContextMenuRequested, &view.settingsView, &settingsView::showContextMenu));
	connectionResults.append(connect(&view.settingsView, &settingsView::deleteUserSignal, &data, &serverModel::deleteUser));

	connectionResults.append(connect(view.settingsView.ui.applyButton, &QPushButton::clicked, &view.settingsView, &settingsView::applySettings));
	connectionResults.append(connect(view.settingsView.ui.saveButton, &QPushButton::clicked, &view.settingsView, &settingsView::saveSettings));
	connectionResults.append(connect(view.settingsView.ui.cancelButton, &QPushButton::clicked, &view, &serverView::closeSettingsMenu));

	connectionResults.append(connect(view.ui.connectedUsersList, &QListWidget::customContextMenuRequested, &view, &serverView::showContextMenu));
	connectionResults.append(connect(&view, &serverView::ForceUserDisconnectSignal, &data, &serverModel::ForceUserDisconnect));

	view.settingsView.menu.addAction("Remove", &view.settingsView, &settingsView::deleteUser);
	view.menu.addAction("Force User Disconnect", &view, &serverView::ForceUserDisconnect);
	view.trayIconMenu.addAction("Exit", &view, &serverView::closeWindow);
}

void serverController::connectModelSignalSlots(QList<bool>& connectionResults)
{
	connectionResults.append(connect(&data, &serverModel::setPortSignal, &view, &serverView::setPort));
	connectionResults.append(connect(&data, &serverModel::writeTextSignal, &view, &serverView::writeTextToScreen));
	connectionResults.append(connect(&data, &serverModel::writeTextSignal, &data.logger, &LoggerManager::logToFile));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::writeTextSignal, &data, &serverModel::writeTextSignal));
	connectionResults.append(connect(&data.networkManager.server, &SslServer::writeTextSignal, &data, &serverModel::writeTextSignal));
	connectionResults.append(connect(&data, &serverModel::connectUserToListSignal, &view, &serverView::connectUserToList));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::connectUserToListSignal, &data, &serverModel::connectUserToListSignal));
	connectionResults.append(connect(&data, &serverModel::deleteUserFromListSignal, &view, &serverView::deleteUserFromList));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::deleteUserFromListSignal, &data, &serverModel::deleteUserFromListSignal));
	connectionResults.append(connect(&data, &serverModel::startServerSignal, &view, &serverView::startServer));
	connectionResults.append(connect(&data, &serverModel::stopServerSignal, &view, &serverView::stopServer));
	connectionResults.append(connect(&data, &serverModel::initializeSettingsSignal, &view.settingsView, &settingsView::initializeSettings));

	connectionResults.append(connect(&data.networkManager.server, &SslServer::newConnection, &data.networkManager, &NetworkManager::newConnectionAttempt));
}


int serverController::init()
{
	data.init();
	view.show();
	return app.exec();
}