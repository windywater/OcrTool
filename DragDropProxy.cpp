#include "DragDropProxy.h"
#include <QMimeData>

DragDropProxy::DragDropProxy(QObject *parent)
	: QObject(parent)
{
	m_proxyWidget = Q_NULLPTR;
}

DragDropProxy::~DragDropProxy()
{
	if (m_proxyWidget)
		removeEventFilter(m_proxyWidget);

	m_proxyWidget = Q_NULLPTR;
}

void DragDropProxy::setProxy(QWidget* proxyWidget)
{
	if (m_proxyWidget)
		removeEventFilter(m_proxyWidget);

	m_proxyWidget = proxyWidget;
	proxyWidget->setAcceptDrops(true);
	m_proxyWidget->installEventFilter(this);
}

bool DragDropProxy::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_proxyWidget)
	{
		QEvent::Type evType = event->type();
		if (evType == QEvent::DragEnter)
		{
			QDragEnterEvent* dragEnterEv = static_cast<QDragEnterEvent*>(event);
			if (dragEnterEv->mimeData()->hasUrls())
			{
				dragEnterEv->acceptProposedAction();
				emit dragEnterTriggered(static_cast<QDragEnterEvent*>(event));
				return true;
			}
		}
		else if (evType == QEvent::DragMove)
			emit dragMoveTriggered(static_cast<QDragMoveEvent*>(event));
		else if (evType == QEvent::DragLeave)
			emit dragLeaveTriggered(static_cast<QDragLeaveEvent*>(event));
		else if (evType == QEvent::Drop)
			emit dropTriggered(static_cast<QDropEvent*>(event));
	}

	return QObject::eventFilter(watched, event);
}
