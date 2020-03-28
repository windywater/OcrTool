#include "SogouOcr.h"
#include <QBuffer>
#include <QImage>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QCryptographicHash>

SogouOcr::SogouOcr(QObject *parent)
	: QObject(parent)
{
	m_mgr = new QNetworkAccessManager(this);
	m_language = "zh-CHS";
}

SogouOcr::~SogouOcr()
{
}

void SogouOcr::setPid(const QString& pid)
{
	m_pid = pid;
}

void SogouOcr::setKey(const QString& key)
{
	m_key = key;
}

void SogouOcr::setLanguage(const QString& language)
{
	m_language = language;
}

void SogouOcr::ocr(const QImage& image)
{
	QNetworkRequest req;
	req.setUrl(QUrl("http://deepi.sogou.com/api/sogouService"));
	req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

	QByteArray service = "basicOpenOcr";
	QByteArray salt = QByteArray::number(QDateTime::currentDateTime().toMSecsSinceEpoch());

	QByteArray imageRaw;
	QBuffer buffer(&imageRaw);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "JPEG");
	QByteArray imageBytes = imageRaw.toBase64();

	QByteArray imageShortBytes = imageBytes.left(1024);
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(m_pid.toLatin1() + service + salt + imageShortBytes + m_key.toLatin1());
	QByteArray sign = hash.result().toHex();

	QUrlQuery query;
	query.addQueryItem("lang", m_language);
	query.addQueryItem("pid", m_pid);
	query.addQueryItem("service", service);
	query.addQueryItem("sign", sign);
	query.addQueryItem("salt", salt);
	query.addQueryItem("image", imageBytes);

	QByteArray payload = query.toString().replace("+", "%2b").toUtf8();
	QNetworkReply* reply = m_mgr->post(req, payload);
	QObject::connect(reply, &QNetworkReply::finished, this, [=]() {
		reply->deleteLater();
		QNetworkReply::NetworkError error = reply->error();
		if (error != QNetworkReply::NoError)
		{
			emit finished((int)error, "Network error");
			return;
		}

		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject rootObj = doc.object();
		if (rootObj["success"].toInt() != 1)
		{
			emit finished(1000, "Sogou response failed");
		}

		QStringList lines;
		QJsonArray resultArray = rootObj["result"].toArray();
		for (int i = 0; i < resultArray.size(); i++)
		{
			QJsonObject obj = resultArray.at(i).toObject();
			lines << obj["content"].toString().trimmed();
		}

		emit finished(0, lines.join("\n"));
	});

}
