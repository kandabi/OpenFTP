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
	connectionResults.append(connect(&view, &clientView::renameInServerSignal, &data, &ClientModel::renameInServer));
	connectionResults.append(connect(&view, &clientView::renameInLocalSignal, &data, &ClientModel::renameInLocal));
	connectionResults.append(connect(&view, &clientView::createNewFolderSignal, &data, &ClientModel::createFolderAction));
	connectionResults.append(connect(&view, &clientView::queueFilesToUploadSignal, &data, &ClientModel::queueFilesToUpload));
	connectionResults.append(connect(view.ui.uploadButton, &QPushButton::clicked, &view, &clientView::uploadFileButton));
	connectionResults.append(connect(view.ui.downloadButton, &QPushButton::clicked, &view, &clientView::downloadFileButton));
	connectionResults.append(connect(&view, &clientView::queueFilesToDownloadSignal, &data, &ClientModel::queueFilesToDownload));
	connectionResults.append(connect(&view, &clientView::copyFilesToDirectorySignal, &data, &ClientModel::copyFilesToDirectory));
	connectionResults.append(connect(&view, &clientView::copyFilesToClipboardLocalSignal, &data, &ClientModel::copyFilesToClipboardLocal));
	connectionResults.append(connect(&view, &clientView::copyFilesToClipboardServerSignal, &data, &ClientModel::copyFilesToClipboardServer));

	connectionResults.append(connect(view.fileExistsWindow.ui.okButton, &QPushButton::clicked , &view.fileExistsWindow, &fileExistsView::performSelection));
	connectionResults.append(connect(&view.fileExistsWindow, &fileExistsView::performSelectionSignal, &data, &ClientModel::fileAlreadyExistsSelection));
	connectionResults.append(connect(view.fileExistsWindow.ui.permanentCheckbox, &QCheckBox::clicked, &view.fileExistsWindow, &fileExistsView::togglePermanentCheckbox));
	connectionResults.append(connect(view.fileExistsWindow.ui.temporaryCheckbox, &QCheckBox::clicked, &view.fileExistsWindow, &fileExistsView::toggleTemporaryCheckbox));
	connectionResults.append(connect(view.settingsWindow.ui.resetFileExistsBehaviorButton, &QPushButton::clicked, &data, &ClientModel::resetFileAlreadyExistsBehavior));
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
	connectionResults.append(connect(&data, &ClientModel::setLocalFileBrowserSignal, &view, &clientView::setLocalFileBrowser));
	//connectionResults << connect(&data, &ClientModel::hideProgressBarSignal, &view, &clientView::hideProgressBar);

	connectionResults << connect(&data, &ClientModel::fileAlreadyExistsSignal, &view, &clientView::fileAlreadyExists);
	connectionResults << connect(&data, &ClientModel::deletedFilesSignal, &view, &clientView::deletedFiles);

}

int clientController::init()
{
	data.init();
	view.show();
	return app.exec();
}