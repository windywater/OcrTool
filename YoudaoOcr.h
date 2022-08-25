#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class YoudaoOcr : public QObject
{
	Q_OBJECT

public:
	YoudaoOcr(QObject *parent);
	~YoudaoOcr();

	void setKey(const QString& key, const QString& secret);
	void ocr(const QImage& image);

Q_SIGNALS:
	void finished(int code, const QString& resultText);

protected:
	QNetworkAccessManager* m_mgr;
	QString m_key;
	QString m_secret;
};
