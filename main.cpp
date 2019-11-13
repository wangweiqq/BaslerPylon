#include "BaslerTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BaslerTest w;
	w.show();
	return a.exec();
}
