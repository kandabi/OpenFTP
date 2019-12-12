#pragma once
#include "stdafx.h"
#include "ui_settingsView.h"

class settingsView : public QDialog
{
	Q_OBJECT

public:
	settingsView(QWidget *parent = Q_NULLPTR);
	~settingsView();

signals:
	void onClose();

private:
	Ui::settingsView ui;
	virtual void reject();

	friend class clientView;
	friend class clientController;
};
