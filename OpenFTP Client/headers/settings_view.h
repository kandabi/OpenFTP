#pragma once
#include "stdafx.h"
#include "ui_settingsView.h"

class settingsView : public QDialog
{
	Q_OBJECT

public:
	settingsView(QWidget *parent = Q_NULLPTR);

	void initSettings(const QString& currentStyle ,const QStringList& styles);
	void selectStyle();

signals:
	void loadStyleSignal(const QString& style);

private:
	Ui::settingsView ui;

	friend class clientView;
	friend class clientController;
};
