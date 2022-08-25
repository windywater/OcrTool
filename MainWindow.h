#pragma once

#include <QtWidgets/QDialog>
#include <QTimer>
#include "ui_MainWindow.h"
#include "OverlappedWidget.h"
#include "qxtglobalshortcut.h"
//#include "SogouOcr.h"
#include "YoudaoOcr.h"

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

protected Q_SLOTS:
	void on_screenOcrButton_clicked();
	void on_setRegionButton_clicked();
	void on_screenshotShortcutApplyButton_clicked();
	void on_ocrInRegionShortcutApplyButton_clicked();
	void on_startStopIntervalOcrButton_clicked();
	void on_clearButton_clicked();

	void onScreenshotShortcutActivated(QxtGlobalShortcut* shortcut);
	void onRegionOcrShortcutActivated(QxtGlobalShortcut* shortcut);
	void onRegionSelected(OverlappedWidget::Action action, const QRect& region);

	void onOcrFinished(int code, const QString& resultText);
	void onIntervalTimerTimeout();
protected:
	void showOverlappedWidget(OverlappedWidget::Action action);
	void doRegionOcr();
	void ocrBufferImages();
	void requestOcr(const QImage& image);
	void showOcrResult(const QString& text);

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void closeEvent(QCloseEvent *event);
private:
	Ui::MainWindowClass ui;
	OverlappedWidget* m_overlappedWidget;
	QxtGlobalShortcut* m_screenshotShortcut;
	QxtGlobalShortcut* m_regionOcrShortcut;
	QRect m_clipRegion;

	YoudaoOcr* m_youdaoOcr;
	QString m_lastOcrResult;
	bool m_isIntervalRunningState;
	QTimer* m_intervalTimer;
	QVector<QImage> m_bufferImages;
};
