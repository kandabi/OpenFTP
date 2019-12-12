#pragma once
#include "stdafx.h"
#include "ui_fileExistsView.h"

class fileExistsView : public QDialog
{
	Q_OBJECT

public:
	fileExistsView(QWidget* parent = Q_NULLPTR);
	void setFileName(QString fileName);

signals:
	void onCloseSignal();
	void performSelectionSignal(const int& selection, const bool& rememberSelectionForever, const bool& rememberForRemainingDownloads);

public slots:
	void performSelection();
	void togglePermanentCheckbox();
	void toggleTemporaryCheckbox();

private:
	Ui::fileExists ui;

	friend class clientView;
	friend class clientController;
	//virtual void reject();
};

