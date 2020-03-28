#pragma once

#include <QDialog>
#include "ui_SettingDialog.h"

class SettingDialog : public QDialog
{
	Q_OBJECT

public:
	SettingDialog(QWidget *parent = Q_NULLPTR);
	~SettingDialog();

protected:
	void init();

protected Q_SLOTS:
	void on_applyLabel_linkActivated(const QString &link);
	void on_okButton_clicked();
	void on_closeButton_clicked();

private:
	Ui::SettingDialog ui;
};
