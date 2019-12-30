#pragma once

#include <QLineEdit>

class ShortcutEdit : public QLineEdit
{
	Q_OBJECT

public:
	ShortcutEdit(QWidget *parent);
	~ShortcutEdit();

protected:
	virtual void keyPressEvent(QKeyEvent* event);
};
