#include "BaslerTest.h"

BaslerTest::BaslerTest(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	mThread = BaslerCameraThread::Instance();
	connect(ui.pushButton, SIGNAL(clicked()), mThread, SLOT(StartGrab()));
	connect(ui.pushButton_2, SIGNAL(clicked()), mThread, SLOT(StopCrab()));
}
