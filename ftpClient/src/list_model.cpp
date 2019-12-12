#include "./headers/stdafx.h"
#include "./headers/list_model.h"

FileListServerModel::FileListServerModel(QList<File>& fileList, QObject* parent ) : QAbstractTableModel(parent), itemData(fileList)
{	

}

int FileListServerModel::rowCount(const QModelIndex& parent) const
{
	return itemData.count();
}

int FileListServerModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 5;
}



QVariant FileListServerModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= itemData.size())
		return QVariant();


	if (role == Qt::DecorationRole && index.column() == 0)
		return QVariant(itemData[index.row()].icon);
	if (role == Qt::DisplayRole)
	{
		if (index.column() == 1)
			return QVariant(itemData[index.row()].fileName);
		else if (index.column() == 2 && itemData[index.row()].fileSize != 0)
			return QVariant(locale.formattedDataSize(itemData[index.row()].fileSize));
		else if (index.column() == 3)
			return QVariant(itemData[index.row()].lastModified);
		else if (index.column() == 4)
			return QVariant(itemData[index.row()].filePath);

		else
			return QVariant();
	}
	else
		return QVariant();


}

QVariant FileListServerModel::headerData(int section, Qt::Orientation orientation,int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	QFont font;
	font.setBold(true);

	if (orientation == Qt::Horizontal)
	{
		if (section == 0)
		{
			return QStringLiteral("");
		}
		if (section == 1)
		{
			return QStringLiteral("Name");
		}
		else if (section == 2)
		{
			return QStringLiteral("Size");
		}
		else if (section == 3) 
		{
			return QStringLiteral("Date Modified");
		}
		else if (section == 4)
		{
			return QStringLiteral("Path On Server");
		}
		else {
			return QStringLiteral("");
		}
	}
	else
	{
		return QVariant();
	}
}