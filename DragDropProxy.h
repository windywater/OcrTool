#pragma once

#include <QObject>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>

class DragDropProxy : public QObject
{
	Q_OBJECT

public:
	DragDropProxy(QObject *parent);
	~DragDropProxy();

	void setProxy(QWidget* proxyWidget);

Q_SIGNALS:
	void dragEnterTriggered(QDragEnterEvent* event);
	void dragMoveTriggered(QDragMoveEvent* event);
	void dragLeaveTriggered(QDragLeaveEvent* event);
	void dropTriggered(QDropEvent* event);

protected:
	bool eventFilter(QObject* watched, QEvent* event);

protected:
	QWidget* m_proxyWidget;
};
