#include "YoudaoOcr.h"
#include <QBuffer>
#include <QImage>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QUuid>
#include <QCryptographicHash>

YoudaoOcr::YoudaoOcr(QObject *parent)
	: QObject(parent)
{
	m_mgr = new QNetworkAccessManager(this);
}

YoudaoOcr::~YoudaoOcr()
{
}

static QByteArray truncateBytes(const QByteArray& bytes)
{
	if (bytes.size() <= 20)
		return bytes;

	QByteArray result = bytes.left(10) + QByteArray::number(bytes.size()) + bytes.right(10);
	return result;
}

static QByteArray encrypt(const QByteArray& plain)
{
	QCryptographicHash hash(QCryptographicHash::Sha256);
	hash.addData(plain);
	QByteArray result = hash.result().toHex();

	return result;
}

void YoudaoOcr::setKey(const QString& key, const QString& secret)
{
	m_key = key;
	m_secret = secret;
}

void YoudaoOcr::ocr(const QImage& image)
{
	QNetworkRequest req;
	req.setUrl(QUrl("http://openapi.youdao.com/ocrapi"));
	req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

	QByteArray imageRaw;
	QBuffer buffer(&imageRaw);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "JPEG");
	QByteArray imageBytes = imageRaw.toBase64();

	QUrlQuery query;
	query.addQueryItem("detectType", "10012");	// 按行识别
	query.addQueryItem("imageType", "1");
	query.addQueryItem("langType", "zh-CHS");
	query.addQueryItem("img", imageBytes);
	query.addQueryItem("docType", "json");
	query.addQueryItem("signType", "v3");
	QByteArray curTime = QByteArray::number(QDateTime::currentDateTime().toSecsSinceEpoch());
	query.addQueryItem("curtime", curTime);
	
	QByteArray salt = QUuid::createUuid().toByteArray();
	QByteArray plainSign = m_key.toLatin1() + truncateBytes(imageBytes) + salt + curTime + m_secret.toLatin1();
	QByteArray sign = encrypt(plainSign);
	query.addQueryItem("sign", sign);
	query.addQueryItem("appKey", m_key.toLatin1());
	query.addQueryItem("salt", salt);

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
		qDebug() << doc;

		QJsonObject rootObj = doc.object();
		if (rootObj["errorCode"].toString() != "0")
		{
			emit finished(1000, "Youdao response failed");
		}

		QStringList lines;
		QJsonObject resultObj = rootObj["Result"].toObject();
		QJsonArray regionArray = resultObj["regions"].toArray();
		for (int i = 0; i < regionArray.size(); i++)
		{
			const QJsonObject& regionObj = regionArray.at(i).toObject();
			QJsonArray linesArray = regionObj["lines"].toArray();
			for (int j = 0; j < linesArray.size(); j++)
			{
				QJsonObject lineObj = linesArray.at(j).toObject();
				lines << lineObj["text"].toString();
			}
		}

		emit finished(0, lines.join("\n"));
	});

}
