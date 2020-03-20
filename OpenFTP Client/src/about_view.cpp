#include "stdafx.h"
#include "about_view.h"

aboutView::aboutView(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);

	ui.versionLabel->setText(QString(APP_VERSION));
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	this->setModal(true);
}


void aboutView::setIcon(const QIcon& icon)
{
	ui.iconLabel->setPixmap(icon.pixmap(45,45));
	setWindowIcon(icon);
}