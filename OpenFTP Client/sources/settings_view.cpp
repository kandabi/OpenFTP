#include "stdafx.h"
#include "settings_view.h"

settingsView::settingsView(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);
}

void settingsView::selectStyle()
{
	emit loadStyleSignal(ui.styleSelectorBox->currentText());
}


void settingsView::initSettings(const QString& currentStyle, const QStringList& styles)
{
	ui.styleSelectorBox->clear();
	ui.styleSelectorBox->addItems(styles);
	ui.styleSelectorBox->setCurrentText(currentStyle);
}

void settingsView::reject()
{
	emit onClose();
	QDialog::reject();
}