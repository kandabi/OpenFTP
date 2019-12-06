#include "./headers/stdafx.h"
#include "./headers/controller.h"

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
	connectionResults.append(connect(view.ui.startServerButton, &QPushButton::clicked, &data, &ServerModel::initServer));
	connectionResults.append(connect(view.ui.stopServerButton, &QPushButton::clicked, &data, &ServerModel::stopServer));
	connectionResults.append(connect(view.ui.actionSettings, &QAction::triggered, &data, &ServerModel::writeUsersToSettingsScreen));
	connectionResults.append(connect(view.ui.actionSettings, &QAction::triggered, &view, &serverView::openSettingsMenu));
	connectionResults.append(connect(view.settingsView.ui.registerUserButton, &QPushButton::clicked, &view.settingsView, &settingsView::registerUser));
	connectionResults.append(connect(&view.settingsView, &settingsView::registerUserSignal, &data, &ServerModel::createUser));
	connectionResults.append(connect(view.settingsView.ui.directorySelector, &QPushButton::clicked, &view.settingsView, &settingsView::selectMainFtpDirectory));
	connectionResults.append(connect(view.settingsView.ui.directoryPermittedSelector, &QPushButton::clicked, &view.settingsView, &settingsView::selectUserFtpDirectory));
	connectionResults.append(connect(&view.settingsView, &settingsView::setFtpDirectorySignal, &data, &ServerModel::saveFtpDirectory));

	connectionResults.append(connect(view.settingsView.ui.userListWidget, &QListWidget::customContextMenuRequested, &view.settingsView, &settingsView::showContextMenu));
	view.settingsView.menu.addAction("Erase", &view.settingsView, &settingsView::deleteUser);
	connectionResults.append(connect(&view.settingsView, &settingsView::deleteUserSignal, &data, &ServerModel::deleteUser));

	connectionResults.append(connect(view.settingsView.ui.applyButton, &QPushButton::clicked, &view.settingsView, &settingsView::applySettings));
	connectionResults.append(connect(view.settingsView.ui.saveButton, &QPushButton::clicked, &view.settingsView, &settingsView::saveSettings));
	connectionResults.append(connect(view.settingsView.ui.cancelButton, &QPushButton::clicked, &view, &serverView::closeSettingsMenu));

	connectionResults.append(connect(view.ui.connectedUsersList, &QListWidget::customContextMenuRequested, &view, &serverView::showContextMenu));
	connectionResults.append(connect(&view, &serverView::disconnectUserSignal, &data, &ServerModel::disconnectUser));
}

void serverController::connectModelSignalSlots(QList<bool>& connectionResults)
{
	connectionResults.append(connect(&data, &ServerModel::writeTextSignal, &view, &serverView::writeTextToScreen));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::writeTextSignal, &data, &ServerModel::writeTextSignal));
	connectionResults.append(connect(&data, &ServerModel::connectUserToListSignal, &view, &serverView::connectUserToList));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::connectUserToListSignal, &data, &ServerModel::connectUserToListSignal));
	connectionResults.append(connect(&data, &ServerModel::deleteUserFromListSignal, &view, &serverView::deleteUserFromList));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::deleteUserFromListSignal, &data, &ServerModel::deleteUserFromListSignal));
	connectionResults.append(connect(&data, &ServerModel::closeSettingsSignal, &view, &serverView::closeSettingsMenu));
	connectionResults.append(connect(&data, &ServerModel::startServerSignal, &view, &serverView::startServer));
	connectionResults.append(connect(&data, &ServerModel::stopServerSignal, &view, &serverView::stopServer));
	connectionResults.append(connect(&data, &ServerModel::initializeSettingsSignal, &view.settingsView, &settingsView::initializeSettings));

	connectionResults.append(connect(&data.networkManager.server, &QTcpServer::newConnection, &data.networkManager, &NetworkManager::newConnectionAttempt));
	//connectionResults << connect(&data.networkManager.server, &QTcpServer::newConnection, &data.networkManager, &NetworkManager::newConnectionAttempt);
}


int serverController::init()
{
	data.writeTextSignal("OpenFTP server 0.1.1, written by kandabi" ,Qt::darkGray);
	view.show();
	return app.exec();
}