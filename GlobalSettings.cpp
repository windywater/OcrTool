#include "GlobalSettings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextStream>

GlobalSettings::GlobalSettings()
	: QObject()
{
	loadSettings();
}

GlobalSettings::~GlobalSettings()
{
	uninit();
}

/*static*/ GlobalSettings* GlobalSettings::instance()
{
	static GlobalSettings inst;
	return &inst;
}

void GlobalSettings::init()
{

}

void GlobalSettings::uninit()
{
	saveSettings();
}

static const QString kSettingFileName("settings.json");

static QString readFromFile(const QString& filename, QString codecName = "")
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QByteArray content = file.readAll();
	file.close();

	if (codecName.isEmpty())
		codecName = "UTF-8";

	QTextCodec* codec = QTextCodec::codecForName(codecName.toLatin1());
	if (!codec)
		return QString();

	return codec->toUnicode(content);
}

static void writeToFile(const QString& filename, const QString& content, QString codecName = "")
{
	if (filename.isEmpty())
		return;

	QFile file(filename);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QTextStream ts(&file);

	if (codecName.isEmpty())
		codecName = "UTF-8";

	QTextCodec* textCodec = QTextCodec::codecForLocale();
	if (codecName == "ANSI")
		textCodec = QTextCodec::codecForLocale();
	else
		textCodec = QTextCodec::codecForName(codecName.toLatin1());

	ts.setCodec(textCodec);
	ts.setGenerateByteOrderMark(true);
	ts << content;
	ts.flush();
	file.close();
}

void GlobalSettings::loadSettings()
{
	QString settingFile = QApplication::applicationDirPath() + "/" + kSettingFileName;
	if (!QFile::exists(settingFile))
		return;

	QString content = readFromFile(settingFile);
	QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
	QJsonObject rootObj = doc.object();

	m_settings[OcrEngine] = rootObj["OcrEngine"].toString();
	m_settings[Pid] = rootObj["Pid"].toString();
	m_settings[Key] = rootObj["Key"].toString();
	m_settings[Language] = rootObj["Language"].toString();
	m_settings[ScreenOcrShortcut] = rootObj["ScreenOcrShortcut"].toString();
}

void GlobalSettings::saveSettings()
{
	QString settingFile = QApplication::applicationDirPath() + "/" + kSettingFileName;
	QJsonObject rootObj;

	rootObj["OcrEngine"] = m_settings[OcrEngine];
	rootObj["Pid"] = m_settings[Pid];
	rootObj["Key"] = m_settings[Key];
	rootObj["Language"] = m_settings[Language];
	rootObj["ScreenOcrShortcut"] = m_settings[ScreenOcrShortcut];

	QJsonDocument doc;
	doc.setObject(rootObj);
	writeToFile(settingFile, QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
}

void GlobalSettings::setSetting(SettingKey key, const QString& value)
{
	QString oldValue = m_settings[key];
	if (oldValue == value)
		return;

	m_settings[key] = value;
	emit settingChanged(key, value);
}

QString GlobalSettings::setting(SettingKey key)
{
	return m_settings.value(key);
}


