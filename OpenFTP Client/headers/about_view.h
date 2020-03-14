#pragma once
#include "stdafx.h"
#include "ui_aboutView.h"

class aboutView : public QDialog
{
	Q_OBJECT

public:
	aboutView(QWidget* parent = Q_NULLPTR);

private:
	Ui::aboutView ui;
};
