#include "stdafx.h"
#include "server_view.h"

serverView::serverView(QWidget *parent) : QMainWindow(parent), settingsManager(parent), systemTrayIcon(parent)
{
	ui.setupUi(this);
	ui.stopServerButton->setDisabled(true);
	
	ui.connectedUsersList->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.portEdit->setValidator(new QIntValidator(0, 65535, this));
	ui.mainTextWindow->setOpenExternalLinks(true);

	icon.addFile(":/alienIcon/images/icon.ico");

	systemTrayIcon.setContextMenu(&trayIconMenu);
	systemTrayIcon.setIcon(icon);
	systemTrayIcon.show();

	static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
	if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)
		&& !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
		&& !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
		&& !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	}
}


void serverView::initServer()
{
	emit initServerSignal(ui.portEdit->text().toInt());
}


void serverView::ForceUserDisconnect()
{
	QModelIndexList selected = ui.connectedUsersList->selectionModel()->selectedRows();
	for (const QModelIndex& index : selected)
	{
		emit ForceUserDisconnectSignal(index.data().toString());
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
	ui.mainTextWindow->append("<span style='color:black'> [" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + "] </span> - <span style='color:" + color.name() + ";'>" + text + "</span>");
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


void serverView::setPort(int port)
{
	ui.portEdit->setText(QString::number(port));
}