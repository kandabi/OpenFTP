#include "./headers/stdafx.h"
#include "./headers/settings_view.h"

settingsView::settingsView(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

settingsView::~settingsView() {}

void settingsView::reject()
{
	emit onClose();
	QDialog::reject();
}