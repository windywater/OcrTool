#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QTimer>
#include <QDir>
#include <QImage>
#include <QFile>
#include <QDebug>
#include "SogouOcr.h"

void writeFile(const QString& filename, const QString& content)
{
	QFile file(filename);
	file.open(QIODevice::Truncate | QIODevice::WriteOnly);
	file.write(content.toUtf8());
	file.close();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator* translator = new QTranslator(&a);
	translator->load(":/ocrtool_zh.qm");
	a.installTranslator(translator);

	a.setApplicationDisplayName(MainWindow::tr("OCR tool"));
	MainWindow w;
	w.show();

#if 0
	QString allText;
	QDir dir("C:\\Users\\liqin\\Desktop\\videocover\\videocover");
	QFileInfoList fis = dir.entryInfoList(QStringList{ "*.png" });
	QTimer* timer = new QTimer(&a);
	QObject::connect(timer, &QTimer::timeout, &a, [=, &fis, &allText]() {
		qDebug() << "timeout";

		if (fis.isEmpty())
		{
			qDebug() << "fis is empty";
			timer->stop();
			return;
		}

		QFileInfo fi = fis.first();
		fis.removeFirst();

		SogouOcr* ocr = new SogouOcr(qApp);
		qDebug() << "start ocr" << fi.fileName();
		QObject::connect(ocr, &SogouOcr::finished, qApp, [=, &allText](int code, const QString& resultText) {
			qDebug() << "ocr finished" << fi.fileName();
			QString result = resultText;
			result.replace("\n", "\r\n");
			allText += result + "\r\n";

			QString outputFile = fi.absolutePath() + "/" + fi.completeBaseName() + ".txt";
			
			writeFile(outputFile, result);
			ocr->deleteLater();
		});

		ocr->ocr(QImage(fi.absoluteFilePath()));
	});

	timer->setInterval(1000);
	timer->start();
#endif

	return a.exec();
}
