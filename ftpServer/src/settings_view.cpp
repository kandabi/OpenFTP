#include "./headers/stdafx.h"
#include "./headers/settings_view.h"

settingsView::settingsView(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);

	ui.applyButton->setEnabled(false);

	ui.userValidationLabel->setText("");
	ui.userValidationLabel->setAlignment(Qt::AlignRight);
	/*ui.groupBox.userValidationLabel->setColor(Qt::darkRed);*/
	ui.userListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

settingsView::~settingsView() {}

void settingsView::initializeSettings(QString directory ,QStringList nameList)
{
	applyButtonClicked = false;

	if (directory.isEmpty())
	{
		ui.registerUserButton->setDisabled(true);
		ui.userValidationLabel->setText("You must select a Main FTP directory before creating User Accounts.");
	}
	else {
		ui.registerUserButton->setDisabled(false);
		ui.directoryInput->setText(directory);
		ui.userValidationLabel->setText("");
	}


	ui.userListWidget->clear();
	for (auto name : nameList)
	{
		ui.userListWidget->addItem(name);

	}
}

void settingsView::registerUser()
{
	//QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	QString username = ui.nameInput->text();
	QString password = ui.passwordInput->text();
	if (username.isEmpty() || password.isEmpty())
	{
		ui.userValidationLabel->setText("Please enter a valid Username and a Password.");
		return;
	}

	auto items = ui.userListWidget->findItems(username, Qt::MatchFlag::MatchExactly);
	
	if (!items.isEmpty())
	{
		ui.userValidationLabel->setText("User already exists.");
		return;
	}

	ui.userValidationLabel->setText("");

	emit registerUserSignal(username, password, ui.directoryPermittedInput->text());
}

void settingsView::showContextMenu(const QPoint& pos)
{
	menu.exec(ui.userListWidget->mapToGlobal(pos));
}


void settingsView::selectMainFtpDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty())
	{
		ui.directoryInput->setText(dir);
		ui.applyButton->setEnabled(true);
	}
}


void settingsView::selectUserFtpDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.directoryInput->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty())
	{
		ui.directoryPermittedInput->setText(dir);

	}
}

void settingsView::deleteUser()
{
	for (int i = 0; i < ui.userListWidget->selectedItems().size(); ++i) {
		// Get curent item on selected row
		int row = ui.userListWidget->currentRow();
		QListWidgetItem* item = ui.userListWidget->takeItem(row);
		delete item;
		emit deleteUserSignal(row);

	}
}


void settingsView::saveSettings()
{
	QString dir = ui.directoryInput->text();
	if (!dir.isEmpty() && !applyButtonClicked)
	{
		emit setFtpDirectorySignal(dir);
	}

	applyButtonClicked = false;
	QDialog::close();
}


void settingsView::applySettings()
{
	QString dir = ui.directoryInput->text();
	if (!dir.isEmpty())
	{
		emit setFtpDirectorySignal(dir);
		ui.applyButton->setEnabled(false);

		applyButtonClicked = true;
	}
}


void settingsView::closeSettingsMenu()
{
	ui.directoryInput->clear();
	QDialog::close();
}