#pragma once

#include <QtWidgets/QDialog>
#include "ui_BaslerTest.h"
#include "BaslerCameraThread.h"
class BaslerTest : public QDialog
{
	Q_OBJECT

public:
	BaslerTest(QWidget *parent = Q_NULLPTR);
//public slots:
//	void on_pushButton_clicked();
//	void on_pushButton_clicked();
private:
	Ui::BaslerTestClass ui;
	BaslerCameraThread* mThread;
};
