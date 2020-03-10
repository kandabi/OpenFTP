#include "stdafx.h"
#include "controller.h"

clientController::clientController(int argc, char* argv[], QWidget* parent) : QObject(parent), app(argc, argv) 
{
	QList<bool> connectionResults;
	
	connectViewSignalSlots(connectionResults);
	connectModelSignalSlots(connectionResults);

	Q_ASSERT(!connectionResults.contains(false));
	Q_UNUSED(connectionResults);
}


void clientController::connectViewSignalSlots(QList<bool> &connectionResults)
{
	connectionResults.append(connect(view.ui.connectButton, &QPushButton::clicked, &view, &clientView::connectToServer));
	connectionResults.append(connect(&view, &clientView::connectToServerSignal, &data, &clientModel::connectToServer));
	connectionResults.append(connect(&data, &clientModel::initClient, &view, &clientView::init));
	connectionResults.append(connect(view.ui.disconnectButton, &QPushButton::clicked, &data, &clientModel::disconnectFromServerButton));
	connectionResults.append(connect(view.ui.homeButton, &QPushButton::clicked, &data, &clientModel::browseHomeServer));
	connectionResults.append(connect(view.ui.localHomeButton, &QPushButton::clicked, &data, &clientModel::browseHomeLocal));
	connectionResults.append(connect(view.ui.returnButton, &QPushButton::clicked, &data, &clientModel::returnToLastFolderInServer));
	connectionResults.append(connect(view.ui.localReturnButton, &QPushButton::clicked, &data, &clientModel::returnToLastFolderInLocal));
	connectionResults.append(connect(view.ui.searchButton, &QPushButton::clicked, &view, &clientView::serverSearchBrowser));
	connectionResults.append(connect(view.ui.localSearchButton, &QPushButton::clicked, &view, &clientView::localSearchBrowser));
	connectionResults.append(connect(view.ui.openFileBrowserButton, &QPushButton::clicked, &view, &clientView::openFileBrowser));
	connectionResults.append(connect(&view, &clientView::searchFolderSignal, &data, &clientModel::searchFolder));
	connectionResults.append(connect(view.ui.actionOptions, &QAction::triggered, &view, &clientView::openOptionMenu));
	connectionResults.append(connect(view.ui.serverBrowser, &QTableView::doubleClicked, &data, &clientModel::onDoubleClickServerBrowser));
	connectionResults.append(connect(view.ui.localBrowser, &QTableView::doubleClicked, &data, &clientModel::onDoubleClickLocalBrowser));
	connectionResults.append(connect(&view, &clientView::serverEnterKeySignal, &data, &clientModel::onDoubleClickServerBrowser));
	connectionResults.append(connect(&view, &clientView::localEnterKeySignal, &data, &clientModel::onDoubleClickLocalBrowser));

	connectionResults.append(connect(view.ui.serverBrowser, &QTableView::customContextMenuRequested, &view, &clientView::showServerContextMenu));
	connectionResults.append(connect(view.ui.localBrowser, &QTableView::customContextMenuRequested, &view, &clientView::showLocalContextMenu));
	connectionResults.append(connect(view.ui.deleteButton, &QPushButton::clicked, &view, &clientView::deleteAtServerBrowser));
	connectionResults.append(connect(view.ui.deleteButtonLocal, &QPushButton::clicked, &view, &clientView::deleteAtLocalBrowser));
	connectionResults.append(connect(&view, &clientView::deleteActionSignal, &data, &clientModel::deleteAction));
	connectionResults.append(connect(&view, &clientView::renameInServerSignal, &data, &clientModel::renameInServer));
	connectionResults.append(connect(&view, &clientView::renameInLocalSignal, &data, &clientModel::renameInLocal));
	connectionResults.append(connect(&view, &clientView::createNewFolderSignal, &data, &clientModel::createFolderAction));
	connectionResults.append(connect(&view, &clientView::queueFilesToUploadSignal, &data, &clientModel::queueFilesToUpload));
	connectionResults.append(connect(view.ui.uploadButton, &QPushButton::clicked, &view, &clientView::uploadFileButton));
	connectionResults.append(connect(view.ui.uploadButton2, &QPushButton::clicked, &view, &clientView::uploadFileButton));
	connectionResults.append(connect(view.ui.downloadButton, &QPushButton::clicked, &view, &clientView::downloadFileButton));
	connectionResults.append(connect(view.ui.downloadButton2, &QPushButton::clicked, &view, &clientView::downloadFileButton));
	connectionResults.append(connect(&view, &clientView::queueFilesToDownloadSignal, &data, &clientModel::queueFilesToDownload));
	connectionResults.append(connect(&view, &clientView::copyFilesToDirectorySignal, &data, &clientModel::copyFilesToDirectory));
	connectionResults.append(connect(&view, &clientView::copyFilesToClipboardLocalSignal, &data, &clientModel::copyFilesToClipboardLocal));
	connectionResults.append(connect(&view, &clientView::copyFilesToClipboardServerSignal, &data, &clientModel::copyFilesToClipboardServer));

	connectionResults.append(connect(view.fileExistsWindow.ui.okButton, &QPushButton::clicked , &view.fileExistsWindow, &fileExistsView::performSelection));
	connectionResults.append(connect(&view.fileExistsWindow, &fileExistsView::cancelTransfersSignal, &data, &clientModel::cancelTransfers));
	connectionResults.append(connect(&view.fileExistsWindow, &fileExistsView::performSelectionSignal, &data, &clientModel::fileAlreadyExistsSelection));
	connectionResults.append(connect(view.fileExistsWindow.ui.permanentCheckbox, &QCheckBox::clicked, &view.fileExistsWindow, &fileExistsView::togglePermanentCheckbox));
	connectionResults.append(connect(view.fileExistsWindow.ui.temporaryCheckbox, &QCheckBox::clicked, &view.fileExistsWindow, &fileExistsView::toggleTemporaryCheckbox));
	connectionResults.append(connect(view.settingsWindow.ui.resetFileExistsBehaviorButton, &QPushButton::clicked, &data, &clientModel::resetFileAlreadyExistsBehavior));
	connectionResults.append(connect(view.settingsWindow.ui.resetConnectionData, &QPushButton::clicked, &data, &clientModel::resetConnectionCredentials));
	connectionResults.append(connect(view.ui.storeInformationCheckbox, &QCheckBox::clicked, &view, &clientView::onSaveConnectionCredentials));
	connectionResults.append(connect(&view, &clientView::saveConnectionCredentialsSignal, &data, &clientModel::saveConnectionCredentials));

	connectionResults.append(connect(view.ui.actionExit, &QAction::triggered, &view, &clientView::close));
	connectionResults.append(connect(view.ui.actionExitIcon, &QAction::triggered, &view, &clientView::close));
	connectionResults.append(connect(view.ui.actionFullscreen, &QAction::triggered, &view, &clientView::toggleFullscreen));
	connectionResults.append(connect(view.ui.actionMinimize, &QAction::triggered, &view, &clientView::minimize));

	connectionResults.append(connect(&view.systemTrayIcon, &QSystemTrayIcon::activated, &view, &clientView::activateTrayIcon));
	connectionResults.append(connect(view.settingsWindow.ui.minimizeToTray, &QCheckBox::clicked, &data, &clientModel::setMinimizeToTray));
}

void clientController::connectModelSignalSlots(QList<bool>& connectionResults)
{
	connectionResults.append(connect(&data, &clientModel::writeTextSignal, &view, &clientView::writeTextToScreen));
	connectionResults.append(connect(&data, &clientModel::writeTextSignal, &data.logger, &LoggerManager::logToFile));
	connectionResults.append(connect(&data, &clientModel::beepSignal, &view, &clientView::beep));
	connectionResults.append(connect(&data, &clientModel::connectedToServerSignal, &view, &clientView::connectedToServer));
	connectionResults.append(connect(&data, &clientModel::updateProgressBarSignal, &view, &clientView::updateProgressBar));
	connectionResults.append(connect(&data, &clientModel::uploadCompleteSignal, &view, &clientView::uploadComplete));
	connectionResults.append(connect(&data, &clientModel::uploadFailedSignal, &view, &clientView::uploadFailed));
	connectionResults.append(connect(&data, &clientModel::setLocalFileBrowserSignal, &view, &clientView::setLocalFileBrowser));
	connectionResults.append(connect(&data, &clientModel::fileAlreadyExistsSignal, &view, &clientView::fileAlreadyExists));
	connectionResults.append(connect(&data, &clientModel::deletedFilesSignal, &view, &clientView::deletedFiles));

	connectionResults.append(connect(&data.networkManager.socket, &QTcpSocket::readyRead, &data.networkManager, &NetworkManager::onReadyRead));
	connectionResults.append(connect(&data.networkManager.socket, &QTcpSocket::stateChanged, &data.networkManager, &NetworkManager::onSocketStateChanged));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::writeTextSignal, &view, &clientView::writeTextToScreen));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::disconnectedFromServerSignal, &view, &clientView::disconnectedFromServer));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::disconnectedFromServerSignal, &data, &clientModel::disconnectedFromServer));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::updateProgressBarSignal, &view, &clientView::updateProgressBar));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::setProgressBarSignal, &view, &clientView::setProgressBar));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::checkRemainingDownloadsSignal, &data, &clientModel::checkRemainingDownloads));
	connectionResults.append(connect(&data.networkManager, &NetworkManager::parseJsonSignal, &data, &clientModel::parseJson));

	timer = new QTimer(this);
	connectionResults.append(connect(timer, &QTimer::timeout, &view, &clientView::refreshServerBrowser));
	connectionResults.append(connect(&view, &clientView::refreshServerBrowserSignal, &data, &clientModel::refreshServerBrowser));
	timer->start(10000);
}

int clientController::init()
{
	data.init();
	view.show();
	return app.exec();
}