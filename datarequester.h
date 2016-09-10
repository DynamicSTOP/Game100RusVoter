#ifndef DATAREQUESTER_H
#define DATAREQUESTER_H

#include <QObject>
#include <QtNetwork>

class dataRequester : public QObject
{
    Q_OBJECT
public:
    explicit dataRequester(QObject *parent = 0);
    QNetworkAccessManager *m;
    QString cookie,_code,agent;
    QNetworkProxy _proxy;
    QTimer deadProxyTimer;
    bool aborted;
    QStringList ratedIps;
    QNetworkReply *lastReply;
    QNetworkProxy proxy;
signals:
    void data(QString s);
    void ConsoleLog(QString);


    void alreadyVotedIp();
    void VoteOk();
    void VoteError();
    void DeadProxy();
    void ForbiddenProxy();

public slots:
    QString getRandomUserAgent();
    void setProxy(QNetworkProxy p);

    void requestCaptcha();
    void requestCode(QString code);
    void requestVote();
    void requestRate();

    QNetworkRequest generateCaptchaRequest();
    QNetworkRequest generateCodeRequest(QString code);
    QNetworkRequest generateVoteRequest();
    QNetworkRequest generateRateRequest();


    void finishedCaptchRequest();
    void finishedCodeRequest();
    void finishedVoteRequest();
    void finishedRateRequest();

    void error(QNetworkReply::NetworkError);
    void ProxyIsDead();

};

#endif // DATAREQUESTER_H
