#include "./headers/stdafx.h"
#include "./headers/settings_view.h"

settingsView::settingsView(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);
}

void settingsView::reject()
{
	emit onClose();
	QDialog::reject();
}