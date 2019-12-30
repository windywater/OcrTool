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

	void ocr(const QImage& image);

Q_SIGNALS:
	void finished(int code, const QString& resultText);

protected:
	QNetworkAccessManager* m_mgr;
};
