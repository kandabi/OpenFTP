#include "./headers/stdafx.h"
#include "./headers/client_view.h"

clientView::clientView(QWidget *parent) : QMainWindow(parent) , serverMouseMenu(parent), serverEmptyMouseMenu(parent), settingsWindow(parent)
{
	ui.setupUi(this);

	setAcceptDrops(true);

	serverMouseMenu.addAction("Rename", this, &clientView::renameAtServer);
	//serverMouseMenu.addAction("Copy", this, &clientView::copyFilesToClipboard);
	serverMouseMenu.addAction("Delete", this, &clientView::deleteAtServerBrowser);


	pasteAction = serverEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	serverEmptyMouseMenu.addAction("New Folder", this, &clientView::createServerFolder);

	localMouseMenu.addAction("Rename", this, &clientView::renameAtLocal);
	localMouseMenu.addAction("Copy", this, &clientView::copyFilesToClipboard);
	localMouseMenu.addAction("Delete", this, &clientView::deleteAtLocalBrowser);

	localEmptyMouseMenu.addAction("Paste", this, &clientView::pasteFilesFromClipboard);
	localEmptyMouseMenu.addAction("New Folder", this, &clientView::createLocalFolder);

	ui.serverBrowser->viewport()->installEventFilter(parent);
	ui.serverBrowser->setContextMenuPolicy(Qt::CustomContextMenu );
	ui.serverBrowser->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.serverBrowser->horizontalHeader()->setHighlightSections(false);
	ui.serverBrowser->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

	ui.localBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.localBrowser->horizontalHeader()->setHighlightSections(false);
	//ui.localBrowser->setDragEnabled(true);

	ui.disconnectButton->setDisabled(true);
	ui.homeButton->setDisabled(true);
	ui.returnButton->setDisabled(true);
	ui.searchButton->setDisabled(true);
	ui.deleteButton->setDisabled(true);
	ui.uploadButton->setDisabled(true);
	ui.downloadButton->setDisabled(true);
	pasteAction->setEnabled(false);

	ui.progressBar->hide();
}


void clientView::fileAlreadyExists(const QString& filename)
{
	//QMessageBox msgBox(this);
	//msgBox.setText(filename + " already exists on the server.\n");
	//msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	////msgBox.setDefaultButton(QMessageBox::Save);
	//int ret = msgBox.exec();
	fileExistsWindow.setFileName(filename + " already exists on the server.");
	fileExistsWindow.show();

}


void clientView::uploadFileButton()
{
	if (connectedToServerBool && !transfersInProgress && !ui.localBrowser->selectionModel()->selectedRows().isEmpty())
	{   //!transfersInProgress
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

//void MainWindow::mousePressEvent(QMouseEvent* event)
//{
//	if (event->button() == Qt::LeftButton
//		&& iconLabel->geometry().contains(event->pos())) {
//
//		QDrag* drag = new QDrag(this);
//		QMimeData* mimeData = new QMimeData;
//
//		mimeData->setText(commentEdit->toPlainText());
//		drag->setMimeData(mimeData);
//		drag->setPixmap(iconPixmap);
//
//		Qt::DropAction dropAction = drag->exec();
//		...
//	}
//}


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
	QMessageBox::StandardButton reply = QMessageBox::question(this, "ftpClient", "Are you sure you wish to delete the selected files?",
										QMessageBox::Yes | QMessageBox::No);

	if (reply != QMessageBox::Yes)
		return;

	QModelIndexList selected = ui.localBrowser->selectionModel()->selectedRows();
	if (!selected.isEmpty() && !transfersInProgress)
		emit deleteActionSignal(selected, false);
}



void clientView::deleteAtServerBrowser()
{
	QMessageBox::StandardButton reply = QMessageBox::question(this, "ftpClient", "Are you sure you wish to delete the selected files?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply != QMessageBox::Yes)
		return;

	QModelIndexList selected = ui.serverBrowser->selectionModel()->selectedRows();
	if(!selected.isEmpty() && !transfersInProgress)
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
		//}
		//else if(!pasteAction->isEnabled())
		//{
		//	beep();
		//}

		//QStringList filesSelected = getFileListFromMimeData(QApplication::clipboard()->mimeData());
		//if (!filesSelected.isEmpty()) 
		//{
		//	if (connectedToServerBool && !transfersInProgress && ui.serverBrowser->hasFocus())
		//	{
		//		emit queueFilesToUploadSignal(filesSelected, transfersInProgress);
		//		transfersInProgress = true;
		//	}
		//	else if (ui.localBrowser->hasFocus())
		//	{
		//		emit copyFilesToDirectorySignal(filesSelected, true);
		//	}
		//}
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
}


void clientView::uploadFailed(QString error)
{
	beep();
	hideProgressBar();
	writeTextToScreen(error, Qt::darkRed);
	transfersInProgress = false;
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

	//emit searchFolderSignal(ui.serverSearchEdit->text(), true);
}




void clientView::setLocalFileBrowser(QFileSystemModel& model)
{

	ui.localBrowser->setModel(&model);

	currentLocalBrowserPath = model.rootPath();
	ui.localBrowser->setRootIndex(model.index(currentLocalBrowserPath));

	//ui.localBrowser->hideColumn(2);
	ui.localBrowser->setColumnWidth(0, 260);
	ui.localBrowser->setColumnWidth(3, 120);
	ui.localSearchEdit->setText(model.rootPath());
}

void clientView::connectedToServer(FileListServerModel* model,const QString& currentDirectory)
{
	if(model != Q_NULLPTR)
		ui.serverBrowser->setModel(model);

	ui.serverBrowser->setColumnWidth(0, 5);
	ui.serverBrowser->setColumnWidth(1, 200);
	ui.serverBrowser->setColumnWidth(3, 120);
	ui.serverBrowser->setColumnWidth(4, 200);
	
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
	ui.serverSearchEdit->clear();
	connectedToServerBool = false;
	pasteAction->setEnabled(false);
	setProgressBar(0);
	hideProgressBar();
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
				beep(); //*** Implement blinking on download button.
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

QStringList clientView::getFileListFromMimeData(const QMimeData* data)
{
	QStringList filesToUpload;
	foreach(const QUrl &url, data->urls()) {
		filesToUpload.append(url.toLocalFile());
	}
	return filesToUpload;
}
