#pragma once

#include <QtWidgets/QDialog>
#include "ui_MainWindow.h"
#include "OverlappedWidget.h"
#include "qxtglobalshortcut.h"
#include "SogouOcr.h"

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

protected Q_SLOTS:
	void on_screenOcrButton_clicked();
	void on_settingButton_clicked();
	void on_clearButton_clicked();

	void onScreenshotShortcutActivated(QxtGlobalShortcut* shortcut);
	void onRegionSelected(const QRect& region);

	void onSogouOcrFinished(int code, const QString& resultText);
protected:
	void applySettings();
	void showOverlappedWidget();
	void doImageFileOcr(const QString& imageFile);
	void doRegionOcr();
	void requestOcr(const QImage& image);
	void showOcrResult(const QString& text);

	virtual void keyPressEvent(QKeyEvent *event);
private:
	Ui::MainWindowClass ui;
	OverlappedWidget* m_overlappedWidget;
	QxtGlobalShortcut* m_screenshotShortcut;
	QRect m_clipRegion;

	SogouOcr* m_sogouOcr;
};
