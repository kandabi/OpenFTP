#pragma once
#include "stdafx.h"
#include "ui_clientView.h"
#include "settings_view.h"
#include "fileExists_view.h"
#include "list_model.h"
#include "file_manager.h"

class clientView : public QMainWindow
{
	Q_OBJECT
public:
	explicit clientView(QWidget* parent = Q_NULLPTR);

	QMenu serverMouseMenu;
	QMenu serverEmptyMouseMenu;
	QMenu localMouseMenu;
	QMenu localEmptyMouseMenu;
	QAction* pasteAction;

signals:
	void deleteActionSignal(const QModelIndexList& index, bool deleteInServer);
	void renameActionSignal(const QModelIndex& index, const QString& newFileName);
	void renameInServerSignal(const QModelIndex& index, const QString& newFileName);
	void renameInLocalSignal(const QString& file, QString& newFileName);
	void createNewFolderSignal(const QString& newFolderName, bool createInServer);
	void searchFolderSignal(const QString& directory, bool searchInServer);
	void serverEnterKeySignal(const QModelIndex& index);
	void localEnterKeySignal(const QModelIndex& index);
	void queueFilesToUploadSignal(const QStringList& files, bool appendMoreFiles);
	void queueFilesToDownloadSignal(const QModelIndexList& indices, bool appendMorefiles);
	void copyFilesToDirectorySignal(const QStringList& files, bool lastFunction, const QString& directoryTocopy = {});
	void copyFilesToClipboardLocalSignal(const QStringList& files);
	void copyFilesToClipboardServerSignal(const QModelIndexList& indices);

public slots:
	void writeTextToScreen(QString text, QColor color = Qt::black);
	void openOptionMenu();
	void connectedToServer(FileListServerModel* model,const QString& currentDirectory);
	void disconnectedFromServer();
	void showServerContextMenu(const QPoint& pos);
	void showLocalContextMenu(const QPoint& pos);
	void deleteAtLocalBrowser();
	void deleteAtServerBrowser();
	void renameAtServer();
	void renameAtLocal();
	void createServerFolder();
	void createLocalFolder();
	void showProgressBar();
	void hideProgressBar();
	void setProgressBar(qint64 bytesTotal);
	void updateProgressBar(qint64 bytesReceived);
	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent* e);
	void uploadComplete();
	void uploadFailed(QString error);
	void mousePressEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event);
	void setLocalFileBrowser(QFileSystemModel& model);
	void localSearchBrowser();
	void serverSearchBrowser();
	void openFileBrowser();
	void beep();
	void fileAlreadyExists(const QString& filename);
	void deletedFiles();
	void uploadFileButton();
	void downloadFileButton();
	void copyFilesToClipboard();
	void pasteFilesFromClipboard();

private:
	QStringList getFileListFromMimeData(const QMimeData* data);

	//QPropertyAnimation* animation;
	bool connectedToServerBool = false;
	bool transfersInProgress = false;
	QString currentLocalBrowserPath;
	QString currentServerBrowserPath;
	

	Ui::clientGui ui;

	fileExistsView fileExistsWindow;
	settingsView settingsWindow;
	friend class clientController;
};
