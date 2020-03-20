#pragma once
#include "stdafx.h"
#include "ui_aboutView.h"

class aboutView : public QDialog
{
	Q_OBJECT

public:
	aboutView(QWidget* parent = Q_NULLPTR);


	void setIcon(const QIcon& icon);

private:
	Ui::aboutView ui;
};
