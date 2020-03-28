#include "MainWindow.h"
#include "DragDropProxy.h"
#include "GlobalSettings.h"
#include "SettingDialog.h"
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

	m_sogouOcr = new SogouOcr(this);
	connect(m_sogouOcr, &SogouOcr::finished, this, &MainWindow::onSogouOcrFinished);

	m_overlappedWidget = new OverlappedWidget(this);
	connect(m_overlappedWidget, &OverlappedWidget::regionSelected, this, &MainWindow::onRegionSelected);
	
	m_screenshotShortcut = new QxtGlobalShortcut;
	connect(m_screenshotShortcut, &QxtGlobalShortcut::activated, this, &MainWindow::onScreenshotShortcutActivated);

	DragDropProxy* proxy = new DragDropProxy(this);
	proxy->setProxy(this);
	connect(proxy, &DragDropProxy::dropTriggered, this, [=](QDropEvent* event) {
		QList<QUrl> urls = event->mimeData()->urls();
		if (urls.isEmpty())
			return;

		doImageFileOcr(urls.first().toLocalFile());
	});

	applySettings();
}

MainWindow::~MainWindow()
{
	delete m_screenshotShortcut;
}

void MainWindow::applySettings()
{
	m_sogouOcr->setPid(GlobalSettings::instance()->setting(GlobalSettings::Pid));
	m_sogouOcr->setKey(GlobalSettings::instance()->setting(GlobalSettings::Key));
	m_sogouOcr->setLanguage(GlobalSettings::instance()->setting(GlobalSettings::Language));
	
	QString sc = GlobalSettings::instance()->setting(GlobalSettings::ScreenOcrShortcut);
	if (!sc.isEmpty())
		m_screenshotShortcut->setShortcut(QKeySequence(sc));
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
	showOverlappedWidget();
}

void MainWindow::on_settingButton_clicked()
{
	SettingDialog dlg(this);
	if (dlg.exec() == QDialog::Rejected)
		return;

	applySettings();
}

void MainWindow::on_clearButton_clicked()
{
	ui.ocrResultEdit->clear();
}

void MainWindow::showOverlappedWidget()
{
	QScreen* screen = QGuiApplication::primaryScreen();
	QImage scrImage = screen->grabWindow(0).toImage();

	m_overlappedWidget->setScreenImage(scrImage);
	m_overlappedWidget->setGeometry(screen->geometry());
	m_overlappedWidget->show();
	m_overlappedWidget->setFocus();
	QApplication::setActiveWindow(m_overlappedWidget);
}

void MainWindow::onScreenshotShortcutActivated(QxtGlobalShortcut* shortcut)
{
	showOverlappedWidget();
}

static QSize adjustImageSize(const QSize& origin, const QSize& standard)
{
	if (origin.width() <= standard.width() && origin.height() <= standard.height())
		return origin;

	float stdRatio = 1.0f * standard.width() / standard.height();
	float originRatio = 1.0f * origin.width() / origin.height();

	if (originRatio > stdRatio)	// 宽高比大，压缩到标准宽度
	{
		return QSize(standard.width(), (int)(standard.width() / originRatio));
	}
	else	// 宽高比小，压缩到标准高度
	{
		return QSize((int)(standard.height() * originRatio), standard.height());
	}
}

void MainWindow::doImageFileOcr(const QString& imageFile)
{
	QImage image(imageFile);
	if (image.isNull())
		return;

	QSize size = adjustImageSize(image.size(), QSize(2048, 2048));
	if (size != image.size())
		image = image.scaled(size, Qt::KeepAspectRatio);

	image = image.convertToFormat(QImage::Format_Grayscale8);
	requestOcr(image);
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

void MainWindow::onRegionSelected(const QRect& region)
{
	QImage regionImage = m_overlappedWidget->regionImage(region);
	requestOcr(regionImage);
}

void MainWindow::requestOcr(const QImage& image)
{
	ui.msgInfo->setText(tr("OCRing..."));

	QImage processedImage = image.convertToFormat(QImage::Format_Grayscale8);
	m_sogouOcr->ocr(processedImage);
}

void MainWindow::onSogouOcrFinished(int code, const QString& resultText)
{
	// 如果对话框在最小化之前是最大化状态，showNormal会恢复原尺寸，因此要这样处理
	if (isMinimized())
		setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
	else
		show();

	raise();

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