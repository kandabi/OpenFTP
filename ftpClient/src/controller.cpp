#include "./headers/stdafx.h"
#include "./headers/controller.h"

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
	connectionResults.append(connect(view.ui.connectButton, &QPushButton::clicked, &data, &ClientModel::connectToServer));
	connectionResults.append(connect(view.ui.disconnectButton, &QPushButton::clicked, &data, &ClientModel::disconnectFromServer));
	connectionResults.append(connect(view.ui.homeButton, &QPushButton::clicked, &data, &ClientModel::browseHome));
	connectionResults.append(connect(view.ui.localHomeButton, &QPushButton::clicked, &data, &ClientModel::localBrowseHome));
	connectionResults.append(connect(view.ui.returnButton, &QPushButton::clicked, &data, &ClientModel::returnToLastFolder));
	connectionResults.append(connect(view.ui.localReturnButton, &QPushButton::clicked, &data, &ClientModel::localReturnToLastFolder));
	connectionResults.append(connect(view.ui.searchButton, &QPushButton::clicked, &view, &clientView::serverSearchBrowser));
	connectionResults.append(connect(view.ui.localSearchButton, &QPushButton::clicked, &view, &clientView::localSearchBrowser));
	connectionResults.append(connect(view.ui.openFileBrowserButton, &QPushButton::clicked, &view, &clientView::openFileBrowser));
	connectionResults.append(connect(&view, &clientView::searchFolderSignal, &data, &ClientModel::searchFolder));
	connectionResults.append(connect(view.ui.actionOptions, &QAction::triggered, &view, &clientView::openOptionMenu));
	connectionResults.append(connect(view.ui.serverBrowser, &QTableView::doubleClicked, &data, &ClientModel::onDoubleClickServerBrowser));
	connectionResults.append(connect(view.ui.localBrowser, &QTableView::doubleClicked, &data, &ClientModel::onDoubleClickLocalBrowser));
	connectionResults.append(connect(&view, &clientView::serverEnterKeySignal, &data, &ClientModel::onDoubleClickServerBrowser));
	connectionResults.append(connect(&view, &clientView::localEnterKeySignal, &data, &ClientModel::onDoubleClickLocalBrowser));
	//results << connect(&window.settingsWindow, &settingsView::onClose, &data, &clientModel::updateData);

	connectionResults.append(connect(view.ui.serverBrowser, &QTableView::customContextMenuRequested, &view, &clientView::showServerContextMenu));
	connectionResults.append(connect(view.ui.localBrowser, &QTableView::customContextMenuRequested, &view, &clientView::showLocalContextMenu));
	connectionResults.append(connect(&view, &clientView::deleteActionSignal, &data, &ClientModel::deleteAction));
	connectionResults.append(connect(view.ui.deleteButton, &QPushButton::clicked, &view, &clientView::deleteAtServerBrowser));
	connectionResults.append(connect(&view, &clientView::renameActionSignal, &data, &ClientModel::renameFile));
	connectionResults.append(connect(&view, &clientView::createNewFolderSignal, &data, &ClientModel::createFolderAction));
	connectionResults.append(connect(&view, &clientView::uploadFileSignal, &data, &ClientModel::uploadFileRequest));
	connectionResults.append(connect(view.ui.uploadButton, &QPushButton::clicked, &view, &clientView::uploadFileButton));
	connectionResults.append(connect(view.ui.downloadButton, &QPushButton::clicked, &view, &clientView::downloadFileButton));
	connectionResults.append(connect(&view, &clientView::downloadFileSignal, &data, &ClientModel::queueFilesToDownload));
}

void clientController::connectModelSignalSlots(QList<bool>& connectionResults)
{
	//connectionResults << connect(&data.socket, &QTcpSocket::connected, &data, &clientModel::connectionEstablished);
	connectionResults.append(connect(&data, &ClientModel::writeTextSignal, &view, &clientView::writeTextToScreen));
	connectionResults.append(connect(&data, &ClientModel::beepSignal, &view, &clientView::beep));
	connectionResults.append(connect(&data.socket, &QTcpSocket::readyRead, &data, &ClientModel::onReadyRead));
	connectionResults.append(connect(&data.socket, &QTcpSocket::stateChanged, &data, &ClientModel::onSocketStateChanged));
	connectionResults.append(connect(&data, &ClientModel::connectedToServerSignal, &view, &clientView::connectedToServer));
	connectionResults.append(connect(&data, &ClientModel::disconnectedFromServerSignal, &view, &clientView::disconnectedFromServer));

	connectionResults.append(connect(&data, &ClientModel::updateProgressBarSignal, &view, &clientView::updateProgressBar));
	connectionResults.append(connect(&data, &ClientModel::setProgressBarSignal, &view, &clientView::setProgressBar));
	connectionResults.append(connect(&data, &ClientModel::uploadCompleteSignal, &view, &clientView::uploadComplete));
	connectionResults.append(connect(&data, &ClientModel::uploadFailedSignal, &view, &clientView::uploadFailed));
	connectionResults.append(connect(&data, &ClientModel::setFileBrowserSignal, &view, &clientView::setFileBrowser));
	//connectionResults << connect(&data, &ClientModel::hideProgressBarSignal, &view, &clientView::hideProgressBar);

	connectionResults << connect(&data, &ClientModel::fileAlreadyExistsSignal, &view, &clientView::fileExists);
	connectionResults << connect(&data, &ClientModel::deletedFilesSignal, &view, &clientView::deletedFiles);

}

int clientController::init()
{
	data.init();
	view.show();
	return app.exec();
}