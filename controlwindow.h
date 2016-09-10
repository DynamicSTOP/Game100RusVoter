#ifndef CONTROLWINDOW_H
#define CONTROLWINDOW_H

#include <QtGui>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets>
#include "breaker.h"
#include "datarequester.h"

class exampleW : public QWidget{
    Q_OBJECT
public:
    exampleW(QWidget* parent=0);
    QList<Number> numbers;
public slots:
    void paintSegments();
protected:
    void paintEvent(QPaintEvent *event);
};


class controlWindow : public QWidget
{
    Q_OBJECT
public:
    explicit controlWindow(QWidget *parent = 0);
    breaker *_br;
    dataRequester *_dr;

private:
    QHBoxLayout *mainLayout;
    QVBoxLayout *layoutButtons;

    QTextEdit *log;
    QPushButton *loadProxyList;
    QPushButton *startWorkBut;
    QList<QNetworkProxy> proxyList;
    QStringList proxyListHosts;
    exampleW *exw;
    int lastProxiId;
    int attempts;

signals:
    void ConsoleLog(QString);
    void ToggleWorkButton();
public slots:
    void drawFigures();
    void loadProxies();

    void setProxy(QNetworkProxy p);
    void startWork();
    void goNextProxy();
};



#endif // CONTROLWINDOW_H
