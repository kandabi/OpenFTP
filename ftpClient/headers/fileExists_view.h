#pragma once
#include "stdafx.h"
#include "ui_fileExistsView.h"

class fileExistsView : public QDialog
{
	Q_OBJECT

public:
	fileExistsView(QWidget* parent = Q_NULLPTR);

signals:
	void onClose();

private:
	Ui::fileExists ui;
	//virtual void reject();
};

