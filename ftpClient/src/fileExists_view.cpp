#include "./headers/stdafx.h"
#include "./headers/fileExists_view.h"

fileExistsView::fileExistsView(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);
}


void fileExistsView::setFileName(QString fileName)
{
	ui.fileExistsLabel->setText(fileName);
}

void fileExistsView::togglePermanentCheckbox()
{
	ui.temporaryCheckbox->setEnabled(!ui.temporaryCheckbox->isEnabled());
}

void fileExistsView::toggleTemporaryCheckbox()
{
	ui.permanentCheckbox->setEnabled(!ui.permanentCheckbox->isEnabled());
}


void fileExistsView::performSelection()
{
	int selection = 0;
	if (ui.overwriteRadioButton->isChecked())
		selection = 1;
	else if (ui.differentNameRadioButton->isChecked())
		selection = 2;
	else if (ui.skipFileRadioButton->isChecked())
		selection = 3;



	emit performSelectionSignal(selection, ui.permanentCheckbox->isChecked(), ui.temporaryCheckbox->isChecked());
}