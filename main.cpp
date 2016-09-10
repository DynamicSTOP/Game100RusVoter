#include <QtGui>
#include <QtCore>
#include <QApplication>
#include <QTextCodec>
#include "controlwindow.h"
#include "stdlib.h"
#include "time.h"

int main(int argc,char** argv){
    QApplication *app = new QApplication(argc,argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

    srand(time(0));
    controlWindow *c = new controlWindow(0);

    c->show();
    app->exec();
    return 0;
}
