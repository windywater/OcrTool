#pragma once

#include <QDialog>
#include <QPaintEvent>
#include <QMouseEvent>

class OverlappedWidget : public QDialog
{
	Q_OBJECT

public:
	OverlappedWidget(QWidget *parent);
	~OverlappedWidget();

	void setScreenImage(const QImage& image);
	QRect selectedRegion();
	QImage regionImage(const QRect& region);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

Q_SIGNALS:
	void regionSelected(const QRect& region);

protected:
	QImage m_screenImage;
	QImage m_darkMaskImage;
	QPoint m_pressedPos;
	QPoint m_draggingPos;
	bool m_dragging;
	QRect m_selectedRegion;
};
