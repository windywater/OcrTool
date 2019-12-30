#include "ShortcutEdit.h"
#include <QKeySequence>
#include <QKeyEvent>
#include <QDebug>

ShortcutEdit::ShortcutEdit(QWidget *parent)
	: QLineEdit(parent)
{
	
}

ShortcutEdit::~ShortcutEdit()
{
}

void ShortcutEdit::keyPressEvent(QKeyEvent* event)
{
	qDebug() << "modifiers:" << event->modifiers();
	qDebug() << "key:" << event->key();

	Qt::KeyboardModifiers modifiers = event->modifiers();
	int keys = event->key();
	QKeySequence keySeq;

	if (modifiers)
	{
		if (event->key() != Qt::Key_Control
			&& event->key() != Qt::Key_Shift
			&& event->key() != Qt::Key_Alt
			&& event->key() != Qt::Key_Meta)
		{
			keySeq = QKeySequence(modifiers + keys);
		}
		else
		{
			keySeq = QKeySequence(modifiers);
		}
	}
	else
	{
		keySeq = QKeySequence(keys);
	}

	setText(keySeq.toString());
}