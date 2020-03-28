#pragma once

#include <QObject>
#include <QMap>

class GlobalSettings : public QObject
{
	Q_OBJECT

private:
	GlobalSettings();
	~GlobalSettings();
	GlobalSettings(const GlobalSettings&);
	GlobalSettings& operator=(const GlobalSettings&);

public:
	enum SettingKey
	{
		OcrEngine,
		Pid,
		Key,
		Language,
		ScreenOcrShortcut
	};

	static GlobalSettings* instance();
	void init();
	void uninit();

	void setSetting(SettingKey key, const QString& value);
	QString setting(SettingKey key);

Q_SIGNALS:
	void settingChanged(SettingKey key, const QString& newSetting);

protected:
	void loadSettings();
	void saveSettings();
	

protected:
	QMap<SettingKey, QString> m_settings;
};
