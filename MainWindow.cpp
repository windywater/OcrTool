#include "MainWindow.h"
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

	m_authOk = false;
	m_sogouOcr = new SogouOcr(this);
	connect(m_sogouOcr, &SogouOcr::finished, this, &MainWindow::onSogouOcrFinished);

	m_overlappedWidget = new OverlappedWidget(this);
	connect(m_overlappedWidget, &OverlappedWidget::regionSelected, this, &MainWindow::onRegionSelected);

	QKeySequence defaultScreenshotShortcut("F7");
	QKeySequence defaultRegionOcrShortcut("F8");

	m_screenshotShortcut = new QxtGlobalShortcut(defaultScreenshotShortcut);
	connect(m_screenshotShortcut, &QxtGlobalShortcut::activated, this, &MainWindow::onScreenshotShortcutActivated);

	m_regionOcrShortcut = new QxtGlobalShortcut(defaultRegionOcrShortcut);
	connect(m_regionOcrShortcut, &QxtGlobalShortcut::activated, this, &MainWindow::onRegionOcrShortcutActivated);

	ui.screenshotShortcutEdit->setText(defaultScreenshotShortcut.toString());
	ui.ocrInRegionShortcutEdit->setText(defaultRegionOcrShortcut.toString());

	auth();
}

MainWindow::~MainWindow()
{
	delete m_screenshotShortcut;
	delete m_regionOcrShortcut;
}

void MainWindow::auth()
{
	QNetworkAccessManager* authMgr = new QNetworkAccessManager(this);
	QNetworkRequest req;
	req.setUrl(QUrl("http://116.62.120.197/auth.txt"));
	QNetworkReply* reply = authMgr->get(req);
	QObject::connect(reply, &QNetworkReply::finished, this, [=]() {
		reply->deleteLater();
		authMgr->deleteLater();

		if (reply->readAll().trimmed() == "1")
			m_authOk = true;
	});
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	if (key == Qt::Key_Escape || key == Qt::Key_Return || key == Qt::Key_Enter)
		return;

	QDialog::keyPressEvent(event);
}

void MainWindow::on_screenOcrButton_clicked()
{
	showOverlappedWidget(OverlappedWidget::OcrInRegion);
}

void MainWindow::on_setRegionButton_clicked()
{
	showOverlappedWidget(OverlappedWidget::ClipRegion);
}

void MainWindow::on_screenshotShortcutApplyButton_clicked()
{
	QKeySequence screenshotShortcut = QKeySequence::fromString(ui.screenshotShortcutEdit->text());
	m_screenshotShortcut->setShortcut(screenshotShortcut);
}

void MainWindow::on_ocrInRegionShortcutApplyButton_clicked()
{
	QKeySequence ocrInRegionShortcut = QKeySequence::fromString(ui.ocrInRegionShortcutEdit->text());
	m_regionOcrShortcut->setShortcut(ocrInRegionShortcut);
}

void MainWindow::on_clearButton_clicked()
{
	ui.ocrResultEdit->clear();
}

void MainWindow::showOverlappedWidget(OverlappedWidget::Action action)
{
	QScreen* screen = QGuiApplication::primaryScreen();
	QImage scrImage = screen->grabWindow(0).toImage();

	m_overlappedWidget->setAction(action);
	m_overlappedWidget->setScreenImage(scrImage);
	m_overlappedWidget->setGeometry(screen->geometry());
	m_overlappedWidget->show();
	m_overlappedWidget->setFocus();
	QApplication::setActiveWindow(m_overlappedWidget);
}

void MainWindow::onScreenshotShortcutActivated(QxtGlobalShortcut* shortcut)
{
	showOverlappedWidget(OverlappedWidget::OcrInRegion);
}

void MainWindow::onRegionOcrShortcutActivated(QxtGlobalShortcut* shortcut)
{
	doRegionOcr();
}

void MainWindow::doRegionOcr()
{
	if (m_clipRegion.isNull())
	{
		QMessageBox::information(this, "", tr("Please set the clip region!"));
		return;
	}

	QScreen* screen = QGuiApplication::primaryScreen();
	QImage clipImage = screen->grabWindow(0, m_clipRegion.left(), m_clipRegion.top(), m_clipRegion.width(), m_clipRegion.height()).toImage();
	requestOcr(clipImage);
}

void MainWindow::onRegionSelected(OverlappedWidget::Action action, const QRect& region)
{
	if (action == OverlappedWidget::OcrInRegion)
	{
		QImage regionImage = m_overlappedWidget->regionImage(region);
		requestOcr(regionImage);
	}
	else if (action == OverlappedWidget::ClipRegion)
	{
		m_clipRegion = region;
	}
}

void MainWindow::requestOcr(const QImage& image)
{
	if (!m_authOk)
	{
		QMessageBox::information(this, "", tr("OCR is not available!"));
		return;
	}

	ui.msgInfo->setText(tr("OCRing..."));

	QImage processedImage = image.convertToFormat(QImage::Format_Grayscale8);
	m_sogouOcr->ocr(processedImage);
}

void MainWindow::onSogouOcrFinished(int code, const QString& resultText)
{
	if (code == 0)
	{
		showOcrResult(resultText);
		ui.msgInfo->setText(tr("OCR finished"));
	}
	else
	{
		ui.msgInfo->setText(resultText);
	}
}

void MainWindow::showOcrResult(const QString& text)
{
	if (!ui.appendCheck->isChecked())
		ui.ocrResultEdit->clear();

	ui.ocrResultEdit->appendPlainText(text + "\n");
	ui.ocrResultEdit->moveCursor(QTextCursor::End);
}