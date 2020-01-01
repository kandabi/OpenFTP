#include "./headers/stdafx.h"
#include "./headers/server_view.h"

serverView::serverView(QWidget *parent) : QMainWindow(parent), settingsManager(parent), systemTrayIcon(parent)
{
	ui.setupUi(this);
	ui.stopServerButton->setDisabled(true);
	
	ui.connectedUsersList->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.portEdit->setValidator(new QIntValidator(0, 65535, this));

	icon.addFile(":/alienIcon/images/icon.png");

	systemTrayIcon.setContextMenu(&trayIconMenu);
	systemTrayIcon.setIcon(icon);
	systemTrayIcon.show();
}


void serverView::initServer()
{
	emit initServerSignal(ui.portEdit->text().toInt());
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


void serverView::activateTrayIcon(QSystemTrayIcon::ActivationReason reason)
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


void serverView::closeEvent(QCloseEvent* event)
{
	if (closing || !settingsManager.getMinimizeToTray())
	{
		ui.stopServerButton->click();
		event->accept();
	}
	else
	{
		this->hide();
		event->ignore();

		if (!settingsManager.getFirstTimeTrayMessage())
		{
			systemTrayIcon.showMessage("OpenFTP server minimized to tray", "Program is still running, you can change this in the settings.", icon);
			settingsManager.setFirstTimeTrayMessage();
		}

	}
}

void serverView::closeWindow()
{
	closing = true;
	close();
}
