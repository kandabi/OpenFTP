#include "stdafx.h"
#include "client_view.h"

clientView::clientView(QWidget* parent) : QMainWindow(parent), serverMouseMenu(parent), serverEmptyMouseMenu(parent), settingsWindow(parent), aboutWindow(parent), fileExistsWindow(parent), systemTrayIcon(parent),
		settingsManager(parent)
{
	ui.setupUi(this);
	setWindowOpacity(0.0);
	fadeInAnimation();
	setAcceptDrops(true);

	menuBar()->addAction("About", this, &clientView::showAboutWindow);
	serverMouseMenu.addAction("Rename", this, &clientView::renameAtServer);
	serverMouseMenu.addAction("Delete", this, &clientView::deleteAtServerBrowser);
	serverEmptyMouseMenu.addAction("New Folder", this, &clientView::createServerFolder);
	localMouseMenu.addAction("Rename", this, &clientView::renameAtLocal);
	localMouseMenu.addAction("Copy", this, &clientView::copyFilesToClipboard);
	localMouseMenu.addAction("Delete", this, &clientView::deleteAtLocalBrowser);
	localEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	localEmptyMouseMenu.addAction("New Folder", this, &clientView::createLocalFolder);
	pasteAction = serverEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	pasteAction->setEnabled(false);

	ui.serverBrowser->viewport()->installEventFilter(parent);
	ui.serverBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.serverBrowser->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.serverBrowser->horizontalHeader()->setHighlightSections(false);
	ui.serverBrowser->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	ui.localBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.localBrowser->horizontalHeader()->setHighlightSections(false);

	disableButtons(true);

	QSizePolicy retainSize = ui.progressBar->sizePolicy();
	retainSize.setRetainSizeWhenHidden(true);
	ui.progressBar->setSizePolicy(retainSize);
	ui.progressBar->hide();

	ui.portEdit->setValidator(new QIntValidator(0, 65535, this));

	appIcon.addFile(":/alienIcon/images/icon_black.ico");
	aboutWindow.setIcon(QIcon(":/alienIcon/images/icon.png"));

	trayIconMenu.addAction("Exit", this, &clientView::closeWindow);
	systemTrayIcon.setContextMenu(&trayIconMenu);
	systemTrayIcon.setIcon(appIcon);
	systemTrayIcon.show();

	statusBar()->addWidget(&statusBarLabel);

	loadStyle(settingsManager.getAppStyle());
}




void clientView::disableButtons(const bool disable)
{
	ui.disconnectButton->setDisabled(disable);
	ui.homeButton->setDisabled(disable);
	ui.returnButton->setDisabled(disable);
	ui.searchButton->setDisabled(disable);
	ui.deleteButton->setDisabled(disable);
	ui.uploadButton->setDisabled(disable);
	ui.uploadButton2->setDisabled(disable);
	ui.downloadButton->setDisabled(disable);
	ui.downloadButton2->setDisabled(disable);
	ui.serverSearchEdit->setDisabled(disable);
}


void clientView::fileAlreadyExists(const QString& filename)
{
	fileExistsWindow.setFileName(filename + " already exists on the server.");
	fileExistsWindow.show();
}


void clientView::uploadFileButton()
{
	if (connectedToServerBool /*&& !transfersInProgress*/ && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
	{   
		QStringList filesToUpload;
		for (const QModelIndex& selected : ui.localBrowser->selectionModel()->selectedRows())
		{
			QString fileName = selected.data().toString();
			if (!fileName.startsWith("."))
			{
				filesToUpload.append(currentLocalBrowserPath + "/" + selected.data().toString());
			}
		}
		
		if(!filesToUpload.isEmpty())
		{
			emit queueFilesToUploadSignal(filesToUpload, transfersInProgress);
			transfersInProgress = true;
		}
	}
}

void clientView::downloadFileButton()
{
	if (connectedToServerBool /*&& !transfersInProgress*/ && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
	{
		emit queueFilesToDownloadSignal(ui.serverBrowser->selectionModel()->selectedRows(), transfersInProgress);
		transfersInProgress = true;
	}
}


void clientView::dropEvent(QDropEvent* e)
{
	QStringList filesSelected = getFileListFromMimeData(e->mimeData());
	if (connectedToServerBool && !transfersInProgress &&  ui.serverBrowser->geometry().contains(e->pos()))
	{
		emit queueFilesToUploadSignal(filesSelected, transfersInProgress);
		transfersInProgress = true;
	}
	else if (ui.localBrowser->geometry().contains(e->pos()))
	{
		emit copyFilesToDirectorySignal(filesSelected, true);
	}
}

void clientView::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void clientView::writeTextToScreen(QString text, QColor color)
{
	QString addColor = (color.isValid()) ? "style='color:" + color.name() + ";'" : "";
	ui.mainTextWindow->append("<span " + addColor + " > [" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + "] - " + text + "</span>");
}

void clientView::showAboutWindow()
{
	aboutWindow.show();
}

void clientView::showOptionsWindow(const QString& currentStyle ,const QStringList& styles)
{
	settingsWindow.initSettings(currentStyle ,styles);
	settingsWindow.show();
}

void clientView::showLocalContextMenu(const QPoint& pos)
{
	if (!currentLocalBrowserPath.isEmpty() && !currentLocalBrowserPath.endsWith(":/Windows"))
	{
		if (!ui.localBrowser->selectionModel()->selectedRows().isEmpty()) {
			localMouseMenu.exec(QCursor::pos());
		}
		else {
			localEmptyMouseMenu.exec(QCursor::pos());
		}
	}
}


void clientView::showServerContextMenu(const QPoint& pos)
{
	if (connectedToServerBool)
		if (!ui.serverBrowser->selectionModel()->selectedRows().isEmpty()){
			serverMouseMenu.exec(QCursor::pos());
		}
		else {
			serverEmptyMouseMenu.exec(QCursor::pos());
		}
}


void clientView::deleteAtLocalBrowser()
{
	QModelIndexList selected = ui.localBrowser->selectionModel()->selectedRows();
	if (selected.isEmpty())
	{
		beep();
		writeTextToScreen("No files have been selected to delete.", Qt::red);
		return;
	}

	QMessageBox::StandardButton reply = QMessageBox::question(this, "openFTP", "Are you sure you wish to delete the selected files?",
										QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes && !transfersInProgress)
		emit deleteActionSignal(selected, false);
}



void clientView::deleteAtServerBrowser()
{
	QModelIndexList selected = ui.serverBrowser->selectionModel()->selectedRows();
	if (selected.isEmpty())
	{
		beep();
		writeTextToScreen("No files have been selected to delete.", Qt::red);
		return;
	}
	QMessageBox::StandardButton reply = QMessageBox::question(this, "openFTP", "Are you sure you wish to delete the selected files?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes && !transfersInProgress)
		emit deleteActionSignal(selected, true);
}


void clientView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::BackButton)
		ui.returnButton->click();
}


void clientView::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();
	switch (key)
	{
		case Qt::Key_Backspace:
			if (ui.localBrowser->hasFocus())
				ui.localReturnButton->click();
			else if (ui.serverBrowser->hasFocus())
				ui.returnButton->click();
		break;
		case Qt::Key_Delete:
			if (ui.localBrowser->hasFocus())
				deleteAtLocalBrowser();
			else if (ui.serverBrowser->hasFocus())
				deleteAtServerBrowser();
		break;
		case Qt::Key_Enter: case Qt::Key_Return:
			if (ui.localSearchEdit->hasFocus())
				localSearchBrowser();
			else if (ui.serverSearchEdit->hasFocus())
				serverSearchBrowser();
			else if (ui.localBrowser->hasFocus() && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
				emit localEnterKeySignal(ui.localBrowser->selectionModel()->selectedRows().first());
			else if (ui.serverBrowser->hasFocus() && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
				emit serverEnterKeySignal(ui.serverBrowser->selectionModel()->selectedRows().first());
		break;
		case Qt::Key_S:
			if (QApplication::keyboardModifiers() && Qt::ControlModifier)
				if (ui.serverBrowser->hasFocus() && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
					renameAtServer();
				else if (ui.localBrowser->hasFocus() && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
					renameAtLocal();
		break;
		case Qt::Key_C:
			if (QApplication::keyboardModifiers() && Qt::ControlModifier)
				copyFilesToClipboard();
		break;
		case Qt::Key_V:
			if(QApplication::keyboardModifiers() && Qt::ControlModifier)
				pasteFilesFromClipboard();
		break;
		case Qt::Key_E:
			if(QApplication::keyboardModifiers() && Qt::ControlModifier)
				uploadFileButton();
		break;
		case Qt::Key_Q:
			if (QApplication::keyboardModifiers() && Qt::ControlModifier)
				downloadFileButton();
		break;
	}
}


void clientView::renameAtServer()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename File"),
		tr("Please enter the text to rename:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty())
		emit renameInServerSignal(ui.serverBrowser->selectionModel()->currentIndex(), text);
}


void clientView::renameAtLocal()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename File"),
		tr("Please enter the text to rename:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty())
		emit renameInLocalSignal(ui.localBrowser->selectionModel()->selectedRows().first().data().toString(), text);
}



void clientView::createServerFolder()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Create Folder"),
		tr("Please enter new folder name:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty())
		emit createNewFolderSignal(currentServerBrowserPath  + "/" + text, true);
}


void clientView::createLocalFolder()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Create Folder"),
		tr("Please enter new folder name:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty())
		emit createNewFolderSignal(currentLocalBrowserPath + "/" + text, false);
}

void clientView::showProgressBar()
{
	ui.progressBar->show();
}

void clientView::hideProgressBar()
{
	ui.progressBar->hide();
}

void clientView::setProgressBar()
{
	ui.progressBar->setValue(0);
	ui.progressBar->setMaximum(100);
	showProgressBar();

	statusBarLabel.setText("Begin Transfer");
}

void clientView::updateProgressBar(const quint64& bytesReceived, const quint64& totalBytes)
{
	ui.progressBar->setValue((quint64)((bytesReceived * 100) / totalBytes));
}


void clientView::uploadComplete()
{
	QSound::play(":/audio/complete.wav");
	hideProgressBar();
	transfersInProgress = false;
	statusBarLabel.setText("Transfer Complete");
}


void clientView::uploadFailed(QString error)
{
	//beep();
	hideProgressBar();
	writeTextToScreen(error, Qt::red);
	transfersInProgress = false;
	statusBarLabel.setText("Transfer Failed");
}

void clientView::deletedFiles()
{
	QSound::play(":/audio/delete.wav");
}


void clientView::beep()
{
	QApplication::beep();
}


void clientView::localSearchBrowser()
{
	emit searchFolderSignal(ui.localSearchEdit->text(), false);
}

void clientView::serverSearchBrowser()
{
	emit searchFolderSignal(ui.serverSearchEdit->text(), true);
}

void clientView::openFileBrowser()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), currentLocalBrowserPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
	if (!dir.isEmpty())
	{
		emit searchFolderSignal(dir, false);
	}
}


void clientView::setLocalFileBrowser(QFileSystemModel& model)
{

	ui.localBrowser->setModel(&model);

	currentLocalBrowserPath = model.rootPath();
	ui.localBrowser->setRootIndex(model.index(currentLocalBrowserPath));

	ui.localBrowser->setColumnWidth(0, 260);
	ui.localBrowser->setColumnWidth(3, 120);
	ui.localSearchEdit->setText(model.rootPath());
}


void clientView::init(const bool& isChecked ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword, const bool& minimizeToTray)
{
	ui.storeInformationCheckbox->setChecked(isChecked);
	ui.addressEdit->setText(serverAddress);
	ui.portEdit->setText(serverPort);
	ui.usernameEdit->setText(userName);
	ui.passwordEdit->setText(userPassword);
	statusBarLabel.setText("Ready");

	settingsWindow.ui.minimizeToTray->setChecked(minimizeToTray);
}

void clientView::connectToServer()
{
	emit connectToServerSignal(ui.addressEdit->text(), ui.portEdit->text(), ui.usernameEdit->text(), ui.passwordEdit->text(), ui.storeInformationCheckbox->isChecked());
}


void clientView::onSaveConnectionCredentials()
{
	bool checked = ui.storeInformationCheckbox->isChecked();
	if (checked)
		emit saveConnectionCredentialsSignal(checked, ui.addressEdit->text(), ui.portEdit->text(), ui.usernameEdit->text(), ui.passwordEdit->text());
	else
		emit saveConnectionCredentialsSignal(checked, "", "", "", "");
}

void clientView::connectedToServer(FileListServerModel* model, const QString& currentDirectory, const QModelIndexList fileIndicesToSelect)
{
	if (model != Q_NULLPTR)
		ui.serverBrowser->setModel(model);
	
	if (!connectedToServerBool) {
		ui.serverBrowser->setColumnWidth(0, 5);
		ui.serverBrowser->setColumnWidth(1, 200);
		ui.serverBrowser->setColumnWidth(2, 60);
		ui.serverBrowser->setColumnWidth(3, 90);
		ui.serverBrowser->setColumnWidth(4, 190);

		statusBarLabel.setText("Connected");
	}
	
	currentServerBrowserPath = currentDirectory;
	ui.serverSearchEdit->setText(currentDirectory);
	ui.connectButton->setDisabled(true);
	disableButtons(false);

	connectedToServerBool = true;

	for (const QModelIndex& index : fileIndicesToSelect)
	{
		ui.serverBrowser->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
	}
}

void clientView::disconnectedFromServer()
{
	ui.serverBrowser->setModel(Q_NULLPTR);
	disableButtons(true);
	ui.connectButton->setDisabled(false);
	ui.serverSearchEdit->clear();
	connectedToServerBool = false;
	pasteAction->setEnabled(false);
	setProgressBar();
	hideProgressBar();
	statusBarLabel.setText("Disconnected");
}


void clientView::pasteFilesFromClipboard()
{
	QStringList filesSelected = getFileListFromMimeData(QApplication::clipboard()->mimeData());
	if (!filesSelected.isEmpty()) 
	{
		if (connectedToServerBool && !transfersInProgress && ui.serverBrowser->hasFocus())
		{
			emit queueFilesToUploadSignal(filesSelected, transfersInProgress);
			transfersInProgress = true;
		}
		else if (ui.localBrowser->hasFocus())
		{
			if (!pasteAction->isEnabled())
			{
				beep(); 
			}
			else {
				emit copyFilesToDirectorySignal(filesSelected, true, currentLocalBrowserPath);
			}
		}
	}
}

void clientView::copyFilesToClipboard()
{
	if (ui.localBrowser->hasFocus() && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
	{
		QStringList filesToUpload;
		for (const QModelIndex& selected : ui.localBrowser->selectionModel()->selectedRows())
		{
			QString fileName = selected.data().toString();
			if (!fileName.startsWith("."))
			{
				filesToUpload.append(currentLocalBrowserPath + "/" + selected.data().toString());
			}
		}
		emit copyFilesToClipboardLocalSignal(filesToUpload);
		pasteAction->setEnabled(true);
	}
	else if (ui.serverBrowser->hasFocus() && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
	{
		emit copyFilesToClipboardServerSignal(ui.serverBrowser->selectionModel()->selectedRows());
		pasteAction->setEnabled(false);
	}
}

void clientView::activateTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (isVisible())
		{
			hide();
		}
		else
		{
			show();
			activateWindow();
		}
	}
}


void clientView::closeEvent(QCloseEvent* event)
{
	if (closing || !settingsManager.getMinimizeToTray())
	{
		ui.disconnectButton->click();
		event->accept();
	}
	else
	{
		this->hide();
		event->ignore();

		if (!settingsManager.getShowTrayMessage())
		{
			systemTrayIcon.showMessage("OpenFTP client minimized to tray", "Program is still running, you can change this in the settings.", appIcon);
			settingsManager.setShowTrayMessage();
		}

	}
}


void clientView::closeWindow()
{
	closing = true;
	close();
}


void clientView::toggleFullscreen()
{
	if (isFullScreen())
	{
		showNormal();
	}
	else {
		showFullScreen();
	}
}

void clientView::minimize()
{
	setWindowState(Qt::WindowMinimized);;
}


void clientView::refreshServerBrowser()
{
	if(connectedToServerBool && !transfersInProgress)
		emit refreshServerBrowserSignal(ui.serverBrowser->selectionModel()->selectedRows());
}

void clientView::fadeInAnimation()
{
	QPropertyAnimation* a1 = new QPropertyAnimation(this, "windowOpacity");
	a1->setDuration(350);
	a1->setStartValue(0.0);
	a1->setEndValue(1.0);
	a1->setEasingCurve(QEasingCurve::Linear);
	a1->start(QPropertyAnimation::DeleteWhenStopped);
}


void clientView::loadStyle(const QString& filename)
{
	QFile File(STYLE_DIR + filename);
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	qApp->setStyleSheet(StyleSheet);

	settingsManager.setAppStyle(filename);
}


void clientView::clearOutputWindow()
{
	ui.mainTextWindow->clear();
}

QStringList clientView::getFileListFromMimeData(const QMimeData* data)
{
	QStringList filesToUpload;
	foreach(const QUrl &url, data->urls()) {
		filesToUpload.append(url.toLocalFile());
	}
	return filesToUpload;
}