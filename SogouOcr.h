#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class SogouOcr : public QObject
{
	Q_OBJECT

public:
	SogouOcr(QObject *parent);
	~SogouOcr();

	void setPid(const QString& pid);
	void setKey(const QString& key);
	void setLanguage(const QString& language);
	void ocr(const QImage& image);

Q_SIGNALS:
	void finished(int code, const QString& resultText);

protected:
	QNetworkAccessManager* m_mgr;
	QString m_pid;
	QString m_key;
	QString m_language;
};
