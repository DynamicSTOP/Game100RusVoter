#include "datarequester.h"

dataRequester::dataRequester(QObject *parent) :
    QObject(parent)
{
    m = new QNetworkAccessManager();
    aborted=false;
    deadProxyTimer.setInterval(120*1000);
    connect(&deadProxyTimer,SIGNAL(timeout()),this,SLOT(ProxyIsDead()));

    ratedIps.clear();
    QFile f("rateIps");
    if(f.exists()){
        f.open(QIODevice::ReadOnly);
        ratedIps.append(QString(f.readAll()).split("\n"));
        f.close();
    }
}

void dataRequester::setProxy(QNetworkProxy p){
    emit ConsoleLog(tr("<font color=\"blue\">Voter new proxy</font> "+p.hostName().toLocal8Bit()+":"+QString::number(p.port()).toLocal8Bit()));
    this->proxy = QNetworkProxy(p);
}

QString dataRequester::getRandomUserAgent(){
    switch(rand()%12){
    case 0:
        return tr("Mozilla/6.0 (Windows NT 6.2; WOW64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 1:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 2:
        return tr("Mozilla/5.0 (Windows NT 6.2; Win64; x64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 3:
        return tr("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:18.0) Gecko/20100101 Firefox/18.0");
        break;
    case 4:
        return tr("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.60 Safari/537.17");
        break;
    case 5:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.15 (KHTML, like Gecko) Chrome/24.0.1295.0 Safari/537.15");
        break;
    case 6:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.14 (KHTML, like Gecko) Chrome/24.0.1292.0 Safari/537.14");
        break;
    case 7:
        return tr("Mozilla/5.0 (compatible; MSIE 10.6; Windows NT 6.1; Trident/5.0; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727) 3gpp-gba UNTRUSTED/1.0");
        break;
    case 8:
        return tr("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; Media Center PC 6.0; InfoPath.3; MS-RTC LM 8; Zune 4.7)");
        break;
    case 9:
        return tr("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; Media Center PC 6.0; InfoPath.3; MS-RTC LM 8; Zune 4.7");
        break;
    case 10:
        return tr("Opera/9.80 (Windows NT 6.0) Presto/2.12.388 Version/12.14");
        break;
    default :
        return tr("Mozilla/5.0 (Windows NT 6.0; rv:2.0) Gecko/20100101 Firefox/4.0 Opera 12.14");
        break;
    }
}

QNetworkRequest dataRequester::generateCaptchaRequest(){
    cookie.clear();
    agent=getRandomUserAgent();
    QNetworkRequest request;
    request.setRawHeader("Host", "game100rus.com");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Referer", "http://game100rus.com/icaptcha.swf");
    request.setRawHeader("x-insight", "activate");
    request.setRawHeader("Accept-Encoding", "deflate");
    request.setRawHeader("User-Agent", agent.toLocal8Bit());

    request.setUrl(QUrl(QString("http://game100rus.com/icaptcha.php?").append(QChar(rand()%25+97)).append(QChar(rand()%25+97))));
    emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" getting captcha data"));
    return request;
}

QNetworkRequest dataRequester::generateCodeRequest(QString code){
    QNetworkRequest request;
    request.setRawHeader(QByteArray("Cookie"),cookie.toLocal8Bit());
    request.setRawHeader("Host", "game100rus.com");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Referer", "http://game100rus.com/index.php?do=votes&id=15839");
    request.setRawHeader("x-insight", "activate");
    request.setRawHeader("Accept-Encoding", "deflate");
    request.setRawHeader("User-Agent", agent.toLocal8Bit());

    request.setUrl(QUrl(QString("http://game100rus.com/icaptcha.php?do=checkcaptcha&c=").append(code)));
    emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" checking captcha"));
    return request;
}

QNetworkRequest dataRequester::generateVoteRequest(){
    QNetworkRequest request;
    request.setRawHeader(QByteArray("Cookie"),cookie.toLocal8Bit());
    request.setRawHeader("Host", "game100rus.com");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Referer", "http://game100rus.com/index.php?do=votes&id=15839");
    request.setRawHeader("x-insight", "activate");
    request.setRawHeader("Accept-Encoding", "deflate");
    request.setRawHeader("User-Agent", agent.toLocal8Bit());

    request.setUrl(QUrl(QString("http://game100rus.com/index.php?do=votes&id=15839")));
    emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" voting..."));
    return request;
}

QNetworkRequest dataRequester::generateRateRequest(){
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Host", "game100rus.com");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("x-insight", "activate");
    request.setRawHeader("Accept-Encoding", "deflate");
    request.setRawHeader("User-Agent", agent.toLocal8Bit());
    request.setUrl(QUrl("http://game100rus.com/index.php?rate"));
    return request;
}


/***************************/

void dataRequester::requestCaptcha(){
    m->setProxy(QNetworkProxy::NoProxy);
    QNetworkReply *r = m->get(generateCaptchaRequest());
    lastReply = r;
    deadProxyTimer.start();
    aborted = false;

    connect(r,SIGNAL(finished()),this,SLOT(finishedCaptchRequest()));
}

void dataRequester::finishedCaptchRequest(){
    if(aborted)
        return;
    aborted = false;
    deadProxyTimer.stop();

    QNetworkReply *r = ((QNetworkReply*)sender());
    QByteArray answer=r->readAll().mid(2);
    cookie.append(QString(r->rawHeader("Set-Cookie")).split(";").first());
    r->deleteLater();
    lastReply=0;
    emit data(answer);
}

/***************************/
void dataRequester::requestCode(QString code){
    _code.clear();_code.append(code);
    m->setProxy(QNetworkProxy::NoProxy);
    QNetworkReply *r = m->get(generateCodeRequest(code));
    lastReply = r;
    deadProxyTimer.start();
    aborted = false;

    connect(r,SIGNAL(finished()),this,SLOT(finishedCodeRequest()));
}

void dataRequester::finishedCodeRequest(){
    if(aborted)
        return;
    aborted = false;
    deadProxyTimer.stop();


    QNetworkReply *r = ((QNetworkReply*)sender());
    QByteArray answer=r->readAll();
    cookie.append(r->rawHeader("Set-Cookie"));

    if(answer.indexOf(QString("INCORRECT"))==-1){
        //captcha ok
        requestVote();
    } else {
        requestCaptcha();
    }
    r->deleteLater();
}
/***************************/
void dataRequester::requestVote(){
    QUrlQuery params;
    params.addQueryItem("vote", "1");
    params.addQueryItem("yes", "Yes");
    params.addQueryItem("captcha", _code.toUtf8());
    QNetworkRequest request = generateVoteRequest();
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    m->setProxy(proxy);
    QNetworkReply *r = m->post(request,params.toString(QUrl::FullyEncoded).toLocal8Bit());
    lastReply = r;
    deadProxyTimer.start();
    aborted = false;

    connect(r,SIGNAL(finished()),this,SLOT(finishedVoteRequest()));
    connect(r,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
}

void dataRequester::finishedVoteRequest(){
    if(aborted)
        return;
    aborted = false;
    deadProxyTimer.stop();

    QNetworkReply *r = ((QNetworkReply*)sender());
    QByteArray answer = r->readAll();

    QVariant status = r->attribute(QNetworkRequest::HttpStatusCodeAttribute);


    if(status.toInt()==200 && QString(answer).indexOf("You have already voted before.")!=-1){
        //proxy was used recently
        emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" You have already voted before"));
        emit alreadyVotedIp();

        lastReply=0;
    } else if(status.toInt()==200 && answer.size()>20000 && answer.indexOf("+ ----- CSS used in default style   ----- +")!=-1){
        //proxy ok
        QFile f("goodProxy.txt");
        f.open(QIODevice::Append);
        QString s;
        s.append(proxy.hostName());
        s.append(":");
        s.append(QString::number(proxy.port()));
        s.append(":");
        if(proxy.type()==QNetworkProxy::HttpProxy)
            s.append("HTTP");
        else
            s.append("socks4/5");
        s.append("\r\n");
        f.write(s.toLocal8Bit());
        f.close();

        if(ratedIps.indexOf(m->proxy().hostName())==-1){
            requestRate();
        }else{
            emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" <font color=\"green\">vote ok</font>"));
            emit VoteOk();
            lastReply=0;
        }
    } else if(answer.indexOf("ERROR: Forbidden")!=-1
              || answer.indexOf("HTTP Proxy Error")!=-1
              || answer.indexOf("Connection timeout")!=-1){
        emit ConsoleLog(tr("<font color=\"red\">BAD PROXY!</font>"));
        emit ForbiddenProxy();
        lastReply=0;
    } else {
        emit VoteError();
        lastReply=0;
    }

    r->deleteLater();
}
/***************************/
void dataRequester::requestRate(){
    int rate=10;
    if(rand()%10>8)
        rate=9;

    QUrlQuery params;
    params.addQueryItem("rate", QString::number(rate).toLocal8Bit());
    params.addQueryItem("id", "15839");

    emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" adding rate -").append(QString::number(rate).toLocal8Bit()));
    QNetworkReply *r = m->post(generateRateRequest(),params.toString(QUrl::FullyEncoded).toLocal8Bit());
    connect(r,SIGNAL(finished()),this,SLOT(finishedRateRequest()));
    lastReply = r;
    deadProxyTimer.start();
    aborted = false;

    ratedIps.append(m->proxy().hostName());
    QFile f("rateIps");
    f.open(QIODevice::WriteOnly);
    f.resize(0);
    f.write(ratedIps.join(tr("\n")).toLocal8Bit());
    f.close();
}

void dataRequester::finishedRateRequest(){
    if(aborted)
        return;
    aborted = false;
    deadProxyTimer.stop();

    QNetworkReply *r = ((QNetworkReply*)sender());
    QByteArray answer = r->readAll();

    QVariant status = r->attribute(QNetworkRequest::HttpStatusCodeAttribute);


    if(status.toInt()==200 && QString(answer).indexOf("You have already voted before.")!=-1){
        //proxy was used recently
        emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" You have already voted before"));
        emit alreadyVotedIp();
    } else if(status.toInt()==200 && answer.size()>20000 && answer.indexOf("+ ----- CSS used in default style   ----- +")!=-1){
        //proxy ok
        emit ConsoleLog(QTime::currentTime().toString(Qt::TextDate).append(" <font color=\"green\">vote ok</font>"));
        emit VoteOk();
    } else {
        emit VoteError();
    }

    r->deleteLater();
    lastReply=0;
}
/***************************/
void dataRequester::error(QNetworkReply::NetworkError err){
    emit ConsoleLog(tr("<font color=\"red\">error code").append(QString::number(err)).append("</font>"));
    ProxyIsDead();
}

void dataRequester::ProxyIsDead(){
    if(!lastReply || aborted)
        return;
    aborted=true;
    deadProxyTimer.stop();
    lastReply->abort();
    lastReply->deleteLater();
    lastReply=0;
    emit ConsoleLog(tr("<font color=\"red\">Dead Proxy</font>"));
    emit DeadProxy();

}
