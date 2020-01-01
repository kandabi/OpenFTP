#include "./headers/stdafx.h"
#include "./headers/client_view.h"

clientView::clientView(QWidget *parent) : QMainWindow(parent, Qt::CustomizeWindowHint) , serverMouseMenu(parent), serverEmptyMouseMenu(parent), settingsWindow(parent), fileExistsWindow(parent), systemTrayIcon(parent), settingsManager(parent)
{
	ui.setupUi(this);
	setWindowOpacity(0.0);
	fadeInAnimation();
	setAcceptDrops(true);

	serverMouseMenu.addAction("Rename", this, &clientView::renameAtServer);
	serverMouseMenu.addAction("Delete", this, &clientView::deleteAtServerBrowser);
	pasteAction = serverEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	serverEmptyMouseMenu.addAction("New Folder", this, &clientView::createServerFolder);
	localMouseMenu.addAction("Rename", this, &clientView::renameAtLocal);
	localMouseMenu.addAction("Copy", this, &clientView::copyFilesToClipboard);
	localMouseMenu.addAction("Delete", this, &clientView::deleteAtLocalBrowser);
	localEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	localEmptyMouseMenu.addAction("New Folder", this, &clientView::createLocalFolder);

	ui.serverBrowser->viewport()->installEventFilter(parent);
	ui.serverBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.serverBrowser->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.serverBrowser->horizontalHeader()->setHighlightSections(false);
	ui.serverBrowser->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	ui.localBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.localBrowser->horizontalHeader()->setHighlightSections(false);

	ui.disconnectButton->setDisabled(true);
	ui.homeButton->setDisabled(true);
	ui.returnButton->setDisabled(true);
	ui.searchButton->setDisabled(true);
	ui.deleteButton->setDisabled(true);
	ui.uploadButton->setDisabled(true);
	ui.downloadButton->setDisabled(true);
	ui.serverSearchEdit->setDisabled(true);
	pasteAction->setEnabled(false);

	QSizePolicy retainSize = ui.progressBar->sizePolicy();
	retainSize.setRetainSizeWhenHidden(true);
	ui.progressBar->setSizePolicy(retainSize);
	ui.progressBar->hide();

	ui.portEdit->setValidator(new QIntValidator(0, 65535, this));
	appIcon.addFile(":/alienIcon/images/icon.png");

	trayIconMenu.addAction("Exit", this, &clientView::closeWindow);
	systemTrayIcon.setContextMenu(&trayIconMenu);
	systemTrayIcon.setIcon(appIcon);
	systemTrayIcon.show();
	statusBar()->addWidget(&statusBarLabel);
	innerHeader = new QMenuBar(menuBar());
	innerHeader->addAction(ui.actionMinimize);
	innerHeader->addAction(ui.actionFullscreen);
	innerHeader->addAction(ui.actionExitIcon);
	menuBar()->setCornerWidget(innerHeader);
	menuBar()->installEventFilter(this);

	QFile File(":/styles/client_style.css");
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	qApp->setStyleSheet(StyleSheet);

	setWindowTitle("OpenFTP client");
}


void clientView::fileAlreadyExists(const QString& filename)
{
	fileExistsWindow.setFileName(filename + " already exists on the server.");
	fileExistsWindow.show();
}


void clientView::uploadFileButton()
{
	if (connectedToServerBool && !transfersInProgress && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
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
	if (connectedToServerBool && !transfersInProgress && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
	{
		transfersInProgress = true;
		emit queueFilesToDownloadSignal(ui.serverBrowser->selectionModel()->selectedRows(), !transfersInProgress);
		
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
	ui.mainTextWindow->setTextColor(color);
	ui.mainTextWindow->append('[' + QDateTime::currentDateTime().toString(Qt::ISODate) + "] - " + text);
}

void clientView::openOptionMenu()
{
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
		return;

	QMessageBox::StandardButton reply = QMessageBox::question(this, "openFTP", "Are you sure you wish to delete the selected files?",
										QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes && !transfersInProgress)
		emit deleteActionSignal(selected, false);
}



void clientView::deleteAtServerBrowser()
{
	QModelIndexList selected = ui.serverBrowser->selectionModel()->selectedRows();
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
	if (event->key() == Qt::Key_Backspace)
	{
		if (ui.localBrowser->hasFocus())
			ui.localReturnButton->click();
		else if (ui.serverBrowser->hasFocus())
			ui.returnButton->click();
	}
	else if (event->key() == Qt::Key_Delete)
	{
		if (ui.localBrowser->hasFocus())
			deleteAtLocalBrowser();
		else if (ui.serverBrowser->hasFocus())
			deleteAtServerBrowser();
	}
	else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		if (ui.localSearchEdit->hasFocus())
			localSearchBrowser();
		else if (ui.serverSearchEdit->hasFocus())
			serverSearchBrowser();
		else if (ui.localBrowser->hasFocus() && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
			emit localEnterKeySignal(ui.localBrowser->selectionModel()->selectedRows().first());
		else if (ui.serverBrowser->hasFocus() && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
			emit serverEnterKeySignal(ui.serverBrowser->selectionModel()->selectedRows().first());
	}
	else if ((event->key() == Qt::Key_S) && QApplication::keyboardModifiers() && Qt::ControlModifier)
	{
		if (ui.serverBrowser->hasFocus() && !ui.serverBrowser->selectionModel()->selectedRows().isEmpty())
		{
			renameAtServer();
		}
		else if (ui.localBrowser->hasFocus() && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
		{
			renameAtLocal();
		}
	}
	else if ((event->key() == Qt::Key_C) && QApplication::keyboardModifiers() && Qt::ControlModifier)
	{
		copyFilesToClipboard();
	}
	else if ((event->key() == Qt::Key_V) && QApplication::keyboardModifiers() && Qt::ControlModifier)
	{
		pasteFilesFromClipboard();
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
	ui.progressBar->setMaximum(0);
}

void clientView::setProgressBar(qint64 bytesTotal)
{
	showProgressBar();
	ui.progressBar->setValue(0);
	ui.progressBar->setMaximum(bytesTotal);

	statusBarLabel.setText("Begin Transfer");
}

void clientView::updateProgressBar(qint64 bytesReceived)
{
	ui.progressBar->setValue(bytesReceived);
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

void clientView::connectedToServer(FileListServerModel* model,const QString& currentDirectory)
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
	ui.disconnectButton->setDisabled(false);
	ui.connectButton->setDisabled(true);
	ui.homeButton->setDisabled(false);
	ui.returnButton->setDisabled(false);
	ui.searchButton->setDisabled(false);
	ui.deleteButton->setDisabled(false);
	ui.uploadButton->setDisabled(false);
	ui.downloadButton->setDisabled(false);
	ui.serverSearchEdit->setDisabled(false);

	connectedToServerBool = true;
}

void clientView::disconnectedFromServer()
{
	ui.serverBrowser->setModel(Q_NULLPTR);
	ui.disconnectButton->setDisabled(true);
	ui.connectButton->setDisabled(false);
	ui.homeButton->setDisabled(true);
	ui.searchButton->setDisabled(true);
	ui.returnButton->setDisabled(true);
	ui.deleteButton->setDisabled(true);
	ui.uploadButton->setDisabled(true);
	ui.downloadButton->setDisabled(true);
	ui.serverSearchEdit->setDisabled(true);
	ui.serverSearchEdit->clear();
	connectedToServerBool = false;
	pasteAction->setEnabled(false);
	setProgressBar(0);
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


bool clientView::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == menuBar())
	{
		auto eventType = event->type();
		if (eventType == QEvent::MouseButtonPress)
		{
			QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
			if (mouse_event->button() == Qt::LeftButton)
			{
				dragPosition = mouse_event->globalPos() - frameGeometry().topLeft();
				performMoveEvent = true;
				return false;
			}
		}
		else if (eventType == QEvent::MouseButtonDblClick)
		{
			if (isMaximized)
			{
				this->setWindowState(Qt::WindowNoState);
			}
			else {
				this->setWindowState(Qt::WindowFullScreen);
			}

			performMoveEvent = false;
			isMaximized = !isMaximized;

			return true;
		}
		else if (eventType == QEvent::MouseMove)
		{
			QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
			if (mouse_event->buttons() & Qt::LeftButton && performMoveEvent)
			{
				move(mouse_event->globalPos() - dragPosition);
				return false;
			}
		}
	}
	return false;
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

QStringList clientView::getFileListFromMimeData(const QMimeData* data)
{
	QStringList filesToUpload;
	foreach(const QUrl &url, data->urls()) {
		filesToUpload.append(url.toLocalFile());
	}
	return filesToUpload;
}