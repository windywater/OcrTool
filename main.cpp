#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator* translator = new QTranslator(&a);
	translator->load(":/ocrtool_zh.qm");
	a.installTranslator(translator);

	a.setApplicationDisplayName(MainWindow::tr("OCR tool"));
	MainWindow w;
	w.show();
	return a.exec();
}
