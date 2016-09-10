#ifndef BREAKER_H
#define BREAKER_H

#include <QtCore>


struct FigurePart{
    QPoint start;
    QPoint end;
    QPoint mid;

    static const double closeDistance = 1.5;
    int averageX();
    int averageY();

    double calcCurvaturePointDistance();
    double static calcCurvaturePointDistance(FigurePart f);
    double static calcDistance(QPoint a, QPoint b);
    bool static isClose(QPoint a, QPoint b);
    bool isClose(FigurePart f2);

};

struct Number{
public:
    QList<FigurePart> parts;

    static const double middleY = 65;

    Number(){
        biggestNode=averageX=0;
        num=-1;
        biggestPoint=QPoint(-1,-1);
        topYtail=250;
        lowYtail=0;
    }

    void append(FigurePart f){parts.append(f);}
    void append(QList<FigurePart> f){parts.append(f);}

    FigurePart takeAt(int i){return parts.takeAt(i);}
    FigurePart takeFirst(){return parts.takeFirst();}
    FigurePart takeLast(){return parts.takeLast();}

    FigurePart get(int i){return parts[i];}
    FigurePart at(int i){return parts.at(i);}
    int length(){return parts.length();}


    bool hasRing();
    bool is0();
    bool is8();
    bool is9();

    bool is7();
    bool is5();
    bool is2();

    int getNumber();
    void setNumber(int n){num=n;}//in case it's 4

    int calcAverageX();
    int static calcAverageY(QList<FigurePart> p);
    int calcBiggestNode();
    QPoint getBiggestPoint();

    int biggestNode,averageX,num,topYtail,lowYtail;
    QPoint biggestPoint;
};

class breaker : public QObject
{
    Q_OBJECT
public:

    explicit breaker(QObject *parent = 0);
    QList<FigurePart> figureParts;
    QList<Number> numbers;

signals:
    void drawFigures();
    void answer(QString);
    void ConsoleLog(QString);
public slots:
    void work(QString coordInfo);
    QString getNumbers();
    void sortFigures();
    void parseFigures();
    void check4();
};

#endif // BREAKER_H
