#include "imagedialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QtSql/QtSql>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageDialog w;
    w.show();

    return a.exec();
}
