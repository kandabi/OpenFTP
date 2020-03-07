#pragma once
#include "stdafx.h"
#include "ui_clientView.h"
#include "settings_view.h"
#include "fileExists_view.h"
#include "list_model.h"
#include "file_manager.h"
#include "settings_manager.h"

class clientView : public QMainWindow
{
	Q_OBJECT
public:
	explicit clientView(QWidget* parent = Q_NULLPTR);

	QMenu serverMouseMenu;
	QMenu serverEmptyMouseMenu;
	QMenu localMouseMenu;
	QMenu localEmptyMouseMenu;
	QMenu trayIconMenu;
	QAction* pasteAction;
	QAction* fullscreenAction;

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
	void connectToServerSignal(const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword, const bool& saveInformation);
	void saveConnectionCredentialsSignal(const bool& isChecked ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword);
	

public slots:
	void onSaveConnectionCredentials();
	void connectToServer();
	void writeTextToScreen(QString text, QColor color = Qt::white);
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
	void setProgressBar();
	void updateProgressBar(const quint64& bytesReceived, const quint64& bytesTotal);
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
	void init(const bool& isChecked ,const QString& serverAddress, const QString& serverPort, const QString& userName, const QString& userPassword, const bool& minimizeToTray);
	void closeEvent(QCloseEvent* event) override;
	void closeWindow();
	void activateTrayIcon(QSystemTrayIcon::ActivationReason reason);
	bool eventFilter(QObject* watched, QEvent* event) override;
	void toggleFullscreen();
	void minimize();

private:
	QStringList getFileListFromMimeData(const QMimeData* data);
	void fadeInAnimation();
	void disableButtons(const bool disable);

	bool isMaximized = false;
	int resizeDetectionRadius = 12;
	int resizeHorizontal = 0;
	int resizeVertical = 0;
	bool performMoveEvent = false;
	QPoint dragPosition;
	QPoint offset;
	int initialFixedCornerPosX;
	int initialFixedCornerPosY;

	bool closing = false;
	bool connectedToServerBool = false;
	bool transfersInProgress = false;
	QString currentLocalBrowserPath;
	QString currentServerBrowserPath;
	
	QSystemTrayIcon systemTrayIcon;
	QIcon appIcon;
	QLabel statusBarLabel;
	QPushButton* exitAction;
	QMenuBar* innerHeader;

	Ui::clientGui ui;

	SettingsManager settingsManager;
	fileExistsView fileExistsWindow;
	settingsView settingsWindow;
	friend class clientController;
};
