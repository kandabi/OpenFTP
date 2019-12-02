#include "./headers/stdafx.h"
#include "./headers/server_view.h"

serverView::serverView(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.stopServerButton->setDisabled(true);

	menu.addAction("Disconnect User", this, &serverView::disconnectUser);
	ui.connectedUsersList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void serverView::closeEvent(QCloseEvent* event)
{
	ui.stopServerButton->click();
	event->accept();
	//if (serverIsRunning)
	//{
	//	QMessageBox::StandardButton resBtn = QMessageBox::question(this, QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
	//		tr("Are you sure you wish to close the server?\n"),
	//		QMessageBox::Cancel | QMessageBox::Yes,
	//		QMessageBox::Yes);
	//	if (resBtn == QMessageBox::Yes) {
	//		ui.stopServerButton->click();
	//		event->accept();
	//	}
	//	else {
	//		event->ignore();
	//	}
	//}
}


void serverView::disconnectUser()
{
	QModelIndexList selected = ui.connectedUsersList->selectionModel()->selectedRows();
	for (const QModelIndex& index : selected)
	{
		emit disconnectUserSignal(index.data().toString());
	}

}


void serverView::showContextMenu(const QPoint& pos)
{
	QModelIndexList selected = ui.connectedUsersList->selectionModel()->selectedRows();
	if (!selected.isEmpty())
		menu.exec(QCursor::pos());
}



void serverView::writeTextToScreen(QString text, QColor color)
{
	ui.mainTextWindow->setTextColor(color);
	ui.mainTextWindow->append('['  + QDateTime::currentDateTime().toString(Qt::ISODate) + "] - " + text);
}

void serverView::openSettingsMenu()
{
	settingsView.ui.nameInput->clear();
	settingsView.ui.passwordInput->clear();
	settingsView.show();
}

void serverView::closeSettingsMenu()
{
	settingsView.hide();
}

void serverView::connectUserToList(QString name)
{
	QListWidgetItem* newUser = new QListWidgetItem;
	newUser->setText(name);
	ui.connectedUsersList->insertItem(ui.connectedUsersList->count(), newUser);
}

void serverView::deleteUserFromList(QString name)
{
	qDeleteAll(ui.connectedUsersList->findItems(name, Qt::MatchFixedString));
}



void serverView::startServer()
{
	ui.startServerButton->setDisabled(true);
	ui.stopServerButton->setDisabled(false);

	serverIsRunning = true;
}


void serverView::stopServer()
{
	writeTextToScreen("Closing FTP Server", Qt::black);
	ui.startServerButton->setDisabled(false);
	ui.stopServerButton->setDisabled(true);

	ui.connectedUsersList->clear();

	serverIsRunning = false;
}
