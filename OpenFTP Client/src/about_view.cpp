#include "stdafx.h"
#include "about_view.h"

aboutView::aboutView(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);
}