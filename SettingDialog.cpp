#include "SettingDialog.h"
#include "GlobalSettings.h"
#include <QDesktopServices>
#include <QUrl>

SettingDialog::SettingDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	init(); 
}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::init()
{
	setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

	ui.ocrEngineCombo->addItem(tr("Sogou"));

	ui.pidEdit->setText(GlobalSettings::instance()->setting(GlobalSettings::Pid));
	ui.keyEdit->setText(GlobalSettings::instance()->setting(GlobalSettings::Key));

	QList<QPair<QString, QString>> langs{
		{ tr("Chinese and English"), "zh-CHS" },
		{ tr("English"), "en" },
		{ tr("Russian"), "ru" },
		{ tr("Korean"), "ko" },
		{ tr("French"), "fr" },
		{ tr("German"), "de" },
		{ tr("Spanish"), "es" },
		{ tr("Portuguese"), "pt" },
	};

	for (const auto& langPair : langs)
		ui.languageCombo->addItem(langPair.first, langPair.second);

	int langIdx = ui.languageCombo->findData(GlobalSettings::instance()->setting(GlobalSettings::Language));
	if (langIdx != -1)
		ui.languageCombo->setCurrentIndex(langIdx);

	ui.screenOcrShortcutEdit->setText(GlobalSettings::instance()->setting(GlobalSettings::ScreenOcrShortcut));
}

void SettingDialog::on_applyLabel_linkActivated(const QString &link)
{
	QDesktopServices::openUrl(QUrl("https://deepi.sogou.com/registered/textcognitive"));
}

void SettingDialog::on_okButton_clicked()
{
	GlobalSettings::instance()->setSetting(GlobalSettings::OcrEngine, ui.ocrEngineCombo->currentText());
	GlobalSettings::instance()->setSetting(GlobalSettings::Pid, ui.pidEdit->text());
	GlobalSettings::instance()->setSetting(GlobalSettings::Key, ui.keyEdit->text());
	GlobalSettings::instance()->setSetting(GlobalSettings::Language, ui.languageCombo->currentData().toString());
	GlobalSettings::instance()->setSetting(GlobalSettings::ScreenOcrShortcut, ui.screenOcrShortcutEdit->text());
	
	accept();
}

void SettingDialog::on_closeButton_clicked()
{
	reject();
}
