#include "OverlappedWidget.h"
#include <QPainter>

OverlappedWidget::OverlappedWidget(QWidget *parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	
	m_dragging = false;
	m_action = None;
}

OverlappedWidget::~OverlappedWidget()
{
}

void OverlappedWidget::setAction(Action action)
{
	m_action = action;
}

void OverlappedWidget::setScreenImage(const QImage& image)
{
	m_screenImage = image;
	if (m_screenImage.format() != QImage::Format_RGB888)
		m_screenImage = m_screenImage.convertToFormat(QImage::Format_RGB888);

	m_darkMaskImage = m_screenImage;
	uchar* bits = m_darkMaskImage.bits();
	for (int i = 0; i < m_darkMaskImage.byteCount(); i++)
		bits[i] /= 2;
}

QRect OverlappedWidget::selectedRegion()
{
	return m_selectedRegion;
}

QImage OverlappedWidget::regionImage(const QRect& region)
{
	return m_screenImage.copy(region);
}

void OverlappedWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawImage(rect(), m_darkMaskImage);

	if (m_dragging)
	{
		QRect clipRect = QRect(m_pressedPos, m_draggingPos).normalized();
		QImage clipImage = m_screenImage.copy(clipRect);
		painter.drawImage(clipRect, clipImage);

		static QPen borderPen(QBrush(QColor(Qt::green)), 1);
		painter.setPen(borderPen);
		painter.drawRect(clipRect);
	}

	QDialog::paintEvent(event);
}

void OverlappedWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_pressedPos = event->pos();
		m_draggingPos = m_pressedPos;
		m_dragging = true;
	}
	else if (event->button() == Qt::RightButton)
	{
		hide();
		m_dragging = false;
	}

	QDialog::mousePressEvent(event);
}

void OverlappedWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging)
	{
		m_draggingPos = event->pos();
		repaint();
	}

	QDialog::mouseMoveEvent(event);
}

void OverlappedWidget::mouseReleaseEvent(QMouseEvent *event)
{
	hide();
	m_dragging = false;

	if (m_draggingPos != m_pressedPos)
	{
		m_selectedRegion = QRect(m_pressedPos, m_draggingPos).normalized();
		emit regionSelected(m_action, m_selectedRegion);
	}
	
	QDialog::mouseReleaseEvent(event);
}

void OverlappedWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		hide();
		m_dragging = false;
	}

	QDialog::keyPressEvent(event);
}