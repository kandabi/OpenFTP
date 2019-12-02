#pragma once
#include "stdafx.h"
#include "file.h"

class FileListServerModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	FileListServerModel(QVector<File>& fileList, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
	QVector<File> itemData;
	QFileIconProvider provider;
	QLocale locale;
};