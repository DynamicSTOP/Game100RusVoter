#include "controlwindow.h"

controlWindow::controlWindow(QWidget *parent) :
    QWidget(parent)
{
    exw = new exampleW(this);

    mainLayout = new QHBoxLayout(this);
    layoutButtons = new QVBoxLayout();
    mainLayout->addLayout(layoutButtons);

    log = new QTextEdit(this);
    log->setMinimumSize(300,280);
    log->setMaximumSize(300,280);
    log->setReadOnly(true);

    loadProxyList = new QPushButton(tr("load proxy list"),this);
    startWorkBut = new QPushButton(tr("start work"),this);

    layoutButtons->addWidget(log);
    layoutButtons->addWidget(exw);
    layoutButtons->addWidget(loadProxyList);
    layoutButtons->addWidget(startWorkBut);

    layoutButtons->setAlignment(log,Qt::AlignTop);
    layoutButtons->setAlignment(loadProxyList,Qt::AlignTop);
    layoutButtons->setAlignment(startWorkBut,Qt::AlignTop);

    mainLayout->setAlignment(layoutButtons,Qt::AlignLeft);
    mainLayout->addSpacing(640-log->width()-layoutButtons->margin()*2-mainLayout->margin()*2);

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/ROHighWizard.png")));
    this->setPalette(palette);
    this->setWindowTitle(tr("Fuck the vote!"));

    this->setMinimumSize(640,512);
    this->setMaximumSize(640,512);


    _dr = new dataRequester();
    _br = new breaker();

    lastProxiId=-1;


    QObject::connect(_dr,SIGNAL(data(QString)),_br,SLOT(work(QString)));
    QObject::connect(_br,SIGNAL(answer(QString)),_dr,SLOT(requestCode(QString)));
    QObject::connect(_br,SIGNAL(drawFigures()),this,SLOT(drawFigures()));


    connect(_dr,SIGNAL(ConsoleLog(QString)),log,SLOT(append(QString)));
    connect(this,SIGNAL(ConsoleLog(QString)),log,SLOT(append(QString)));
    connect(_br,SIGNAL(ConsoleLog(QString)),log,SLOT(append(QString)));

    connect(_dr,SIGNAL(alreadyVotedIp()),this,SLOT(goNextProxy()));
    connect(_dr,SIGNAL(DeadProxy()),this,SLOT(goNextProxy()));
    connect(_dr,SIGNAL(VoteError()),this,SLOT(goNextProxy()));
    connect(_dr,SIGNAL(VoteOk()),this,SLOT(goNextProxy()));
    connect(_dr,SIGNAL(ForbiddenProxy()),this,SLOT(goNextProxy()));



    //
    connect(startWorkBut,SIGNAL(clicked()),this,SLOT(startWork()));
    connect(startWorkBut,SIGNAL(clicked()),startWorkBut,SLOT(hide()));
    connect(this,SIGNAL(ToggleWorkButton()),startWorkBut,SLOT(show()));

    connect(loadProxyList,SIGNAL(clicked()),this,SLOT(loadProxies()));

}

void controlWindow::drawFigures(){
    exw->numbers=_br->numbers;
    exw->update();
}

void controlWindow::loadProxies(){
    QString fileName = QFileDialog::getOpenFileName(0,
         tr("Open proxy list"), QDir::currentPath(), tr("Text files (*.txt)"));
    if(fileName.isEmpty())
        return;
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly)){
        emit ConsoleLog(tr("Cannot open file T____T HELP!"));
        return;
    }
    QByteArray arr = f.readAll().replace("\r","");
    QStringList dataStrings = QString(arr).split("\n");
    f.close();
    if(dataStrings.size()==0){
        emit ConsoleLog(tr("No proxy info there >___<"));
        return;
    }

    for(int i=0; i<dataStrings.size();i++){
        QStringList proxyInfo = dataStrings.at(i).split(":");
        if(proxyInfo.size()<2 || proxyInfo.size()>3)
            continue;

        if(proxyListHosts.indexOf(proxyInfo.at(0))!=-1)
            continue;

        QNetworkProxy p;
        p.setHostName(proxyInfo.at(0));

        bool ok;
        p.setPort(proxyInfo.at(1).toInt(&ok));
      /*  if(p.port()!=80 && p.port()!=8080)
            continue;/**/

        if(!ok)
            continue;

        if(proxyInfo.size()==3){
            if(proxyInfo.at(2).toLocal8Bit()=="HTTP" || proxyInfo.at(2).toLocal8Bit()=="HTTPS")
                p.setType(QNetworkProxy::HttpProxy);
            else if(proxyInfo.at(2).indexOf("socks")!=-1)
                p.setType(QNetworkProxy::Socks5Proxy);
            else continue;
        } else {
            p.setType(QNetworkProxy::HttpProxy);
        }

        proxyListHosts.append(proxyInfo.at(0));
        proxyList.append(p);
    }
    emit ConsoleLog(tr("Current proxy count: "+QString::number(proxyList.size()).toLocal8Bit()));
}

void controlWindow::setProxy(QNetworkProxy p){
    _dr->setProxy(p);
}

void controlWindow::goNextProxy(){
    lastProxiId++;
    if(proxyList.size()<=lastProxiId){
        emit ConsoleLog(tr("<font color=\"red\">No more proxies</font>"));
        emit ToggleWorkButton();
        return;
    }
    setProxy(proxyList.at(lastProxiId));
    startWork();
}
void controlWindow::startWork(){
    _br->numbers.clear();
    _br->figureParts.clear();
    _dr->requestCaptcha();

}

/***************************************************************/
exampleW::exampleW(QWidget* parent):QWidget(parent){
    this->setMinimumSize(300,120);
    this->setMaximumSize(300,120);
    this->show();
}

void exampleW::paintEvent(QPaintEvent *event){
    paintSegments();
    QWidget::paintEvent(event);
}

void exampleW::paintSegments(){
    int num = 0;
    QPainter painter(this);
    QPen linepen(Qt::red);
    linepen.setCapStyle(Qt::RoundCap);
    linepen.setWidth(2);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(linepen);


    for(int j=0;j<numbers.length();j++){
        for(int i=0;i<numbers[j].length();i++){

            linepen.setColor(Qt::black);
            painter.setPen(linepen);
            painter.drawLine(numbers[j].get(i).start,numbers[j].get(i).end);

            linepen.setColor(Qt::red);
            painter.setPen(linepen);
            painter.drawLine(numbers[j].get(i).start,numbers[j].get(i).mid);

            linepen.setColor(Qt::darkGreen);
            painter.setPen(linepen);
            painter.drawLine(numbers[j].get(i).end,numbers[j].get(i).mid);

            linepen.setColor(Qt::blue);
            painter.setPen(linepen);
            painter.drawPoint(numbers[j].get(i).mid);
            num++;
        }
    }
}
