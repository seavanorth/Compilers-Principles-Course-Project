#include "stdafx.h"
#include "FrontEndWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrontEndWindow w;
    w.show();
    return a.exec();
}
