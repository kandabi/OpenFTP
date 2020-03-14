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
	void cancelTransfersSignal();

public slots:
	void performSelection();
	void togglePermanentCheckbox();
	void toggleTemporaryCheckbox();

private:
	virtual void reject();

	Ui::fileExistsView ui;

	friend class clientView;
	friend class clientController;
};

