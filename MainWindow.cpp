#include "MainWindow.h"
#include <QScreen>
#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <QDebug>

static int calEditDistance(const QString& str1, const QString& str2)
{
	int len1 = str1.size();
	int len2 = str2.size();
	int* v = new int[len2 + 1];
	int i, j, current, next, cost;

	const ushort* unicode1 = str1.utf16();
	const ushort* unicode2 = str2.utf16();

	int minLen = qMin(len1, len2);
	for (int i = 0; i < minLen; i++)
	{
		if (unicode1[i] == unicode2[i])
		{
			unicode1++;
			unicode2++;
			len1--;
			len2--;
		}
		else
			break;
	}

	if (len1 == 0)
		return len2;

	if (len2 == 0)
		return len1;

	for (int i = 0; i < len2 + 1; i++)
		v[i] = i;

	for (int i = 0; i < len1; i++)
	{
		current = i + 1;
		for (int j = 0; j < len2; j++)
		{
			if (unicode1[i] == unicode2[j] || (i && j &&
				unicode1[i - 1] == unicode2[j] && unicode1[i] == unicode2[j - 1]))
				cost = 0;
			else
				cost = 1;

			next = qMin(qMin(v[j + 1] + 1, current + 1), v[j] + cost);
			v[j] = current;
			current = next;
		}

		v[len2] = next;
	}

	delete[] v;
	return next;
}

static float calStringSimilarity(const QString& str1, const QString& str2)
{
	if (str1.isEmpty() && str2.isEmpty())
		return 1;

	return 1 - 1.0f * calEditDistance(str1, str2) / qMax(str1.size(), str2.size());
}

static QString removeDuplicatedLines(const QString& text)
{
	QStringList lines = text.split("\n", QString::SkipEmptyParts);
	for (int i = lines.size() - 1; i >= 0; i--)
	{
		const QString& line = lines.at(i);
		for (int j = 1; j <= 3; j++)
		{
			int k = i - j;
			if (k < 0)
				break;

			const QString& prevLine = lines.at(k);
			if (calStringSimilarity(line, prevLine) >= 0.8f)
			{
				lines.removeAt(i);
				break;
			}
		}
	}

	QString result = lines.join("\n");
	return result;
}

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

	m_youdaoOcr = new YoudaoOcr(this);
	m_youdaoOcr->setKey("", "");		// TODO: set youdao key and secret
	connect(m_youdaoOcr, &YoudaoOcr::finished, this, &MainWindow::onOcrFinished);

	m_overlappedWidget = new OverlappedWidget(this);
	connect(m_overlappedWidget, &OverlappedWidget::regionSelected, this, &MainWindow::onRegionSelected);

	m_isIntervalRunningState = false;
	m_intervalTimer = new QTimer(this);
	connect(m_intervalTimer, &QTimer::timeout, this, &MainWindow::onIntervalTimerTimeout);

	QKeySequence defaultScreenshotShortcut("F7");
	QKeySequence defaultRegionOcrShortcut("F8");

	m_screenshotShortcut = new QxtGlobalShortcut(defaultScreenshotShortcut);
	connect(m_screenshotShortcut, &QxtGlobalShortcut::activated, this, &MainWindow::onScreenshotShortcutActivated);

	m_regionOcrShortcut = new QxtGlobalShortcut(defaultRegionOcrShortcut);
	connect(m_regionOcrShortcut, &QxtGlobalShortcut::activated, this, &MainWindow::onRegionOcrShortcutActivated);

	ui.screenshotShortcutEdit->setText(defaultScreenshotShortcut.toString());
	ui.ocrInRegionShortcutEdit->setText(defaultRegionOcrShortcut.toString());
}

MainWindow::~MainWindow()
{
	delete m_screenshotShortcut;
	delete m_regionOcrShortcut;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	int key = event->key();
	if (key == Qt::Key_Escape || key == Qt::Key_Return || key == Qt::Key_Enter)
		return;

	QDialog::keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (QMessageBox::question(this, "", tr("Are you sure to quit?")) == QMessageBox::No)
		event->ignore();
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

void MainWindow::on_startStopIntervalOcrButton_clicked()
{
	if (m_isIntervalRunningState)
	{
		ui.startStopIntervalOcrButton->setText(tr("Start interval OCR"));
		m_intervalTimer->stop();
		ocrBufferImages();
		m_isIntervalRunningState = false;

		ui.msgInfo->clear();
	}
	else
	{
		int interval = ui.intervalEdit->text().toInt();
		if (interval < 100)
		{
			QMessageBox::information(this, "", tr("The interval is too short!"));
			return;
		}

		if (m_clipRegion.isNull())
		{
			QMessageBox::information(this, "", tr("Please set the clip region!"));
			return;
		}

		ui.startStopIntervalOcrButton->setText(tr("Stop interval OCR"));
		m_intervalTimer->start(interval);
		m_isIntervalRunningState = true;
	}
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
	ui.msgInfo->setText(tr("OCRing..."));

	QImage processedImage = image.convertToFormat(QImage::Format_Grayscale8);
	m_youdaoOcr->ocr(processedImage);
}

void MainWindow::onOcrFinished(int code, const QString& resultText)
{
	if (code == 0)
	{
		showOcrResult(resultText);
		ui.msgInfo->setText(tr("OCR finished"));
		m_lastOcrResult = resultText;
	}
	else
	{
		ui.msgInfo->setText(resultText);
	}
}

void MainWindow::ocrBufferImages()
{
	QImage firstImage = m_bufferImages.first();
	QImage mergedImage(firstImage.width(), firstImage.height() * m_bufferImages.size(), firstImage.format());
	QPainter painter(&mergedImage);

	for (int i = 0; i < m_bufferImages.size(); i++)
	{
		int top = i * firstImage.height();
		painter.drawImage(0, top, m_bufferImages.at(i));
	}

	m_bufferImages.clear();
	requestOcr(mergedImage);
}

void MainWindow::onIntervalTimerTimeout()
{
	int groupCount = 2048 / m_clipRegion.height();
	if (m_bufferImages.size() < groupCount)
	{
		QScreen* screen = QGuiApplication::primaryScreen();
		QImage clipImage = screen->grabWindow(0, m_clipRegion.left(), m_clipRegion.top(), m_clipRegion.width(), m_clipRegion.height()).toImage();
		m_bufferImages << clipImage;

		ui.msgInfo->setText(tr("Push image buffer %1/%2").arg(m_bufferImages.size()).arg(groupCount));
	}
	else
	{
		if (m_bufferImages.isEmpty())
			return;

		ocrBufferImages();
	}
}

void MainWindow::showOcrResult(const QString& text)
{
	QString processedText = text;
	processedText = removeDuplicatedLines(text);

	if (!ui.appendCheck->isChecked())
		ui.ocrResultEdit->clear();

	ui.ocrResultEdit->appendPlainText(processedText + "\n");
	ui.ocrResultEdit->moveCursor(QTextCursor::End);
}