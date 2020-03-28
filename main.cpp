#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "GlobalSettings.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator* translator = new QTranslator(&a);
	translator->load(":/ocrtool_zh.qm");
	a.installTranslator(translator);

	GlobalSettings::instance()->init();
	a.setApplicationDisplayName(MainWindow::tr("OCR tool"));
	MainWindow w;
	w.show();

	int ret = a.exec();
	GlobalSettings::instance()->uninit();

	return ret;
}
