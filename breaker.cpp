#include "breaker.h"
#include "cmath"

bool operator== (FigurePart p1, FigurePart p2) {
    return (p1.start == p2.start && p1.end==p2.end && p1.mid == p2.mid);
}

int FigurePart::averageX(){
    return (start.x()+mid.x()+end.x())/3;
}
int FigurePart::averageY(){
    return (start.y()+mid.y()+end.y())/3;
}

double FigurePart::calcCurvaturePointDistance(){
    //prevent inf
    if(start.x()==end.x())
        return abs(start.x()-mid.x());

    //Ax+By+C=0
    double A = ((double)(start.y()-end.y()))/((double)(start.x()-end.x()));
    double B = -1;
    double C = (start.y()-A*start.x());
    // |A*Mx+B*My+C| / sqrt(A^2 + B^2)
    return (A*mid.x()+B*mid.y()+C)/((double)sqrt(A*A+B*B));
}

double FigurePart::calcCurvaturePointDistance(FigurePart f){
    //prevent inf
    if(f.start.x()==f.end.x())
        return abs(f.start.x()-f.mid.x());

    //Ax+By+C=0
    double A = (((double)f.start.y()-f.end.y()))/((double)(f.start.x()-f.end.x()));
    double B = -1;
    double C = (f.start.y()-A*f.start.x());
    // |A*Mx+B*My+C| / sqrt(A^2 + B^2)
    return abs(A*f.mid.x()+B*f.mid.y()+C)/((double)sqrt(pow(A,2)+pow(B,2)));
}

double FigurePart::calcDistance(QPoint a, QPoint b){
    return sqrt( pow( (a.x()-b.x()), 2 ) + pow((a.y()-b.y()), 2));
}

bool FigurePart::isClose(QPoint a, QPoint b){
    return calcDistance(a,b) <= closeDistance;
}

bool FigurePart::isClose(FigurePart f2){
    return ( isClose(start,f2.start) ||  isClose(start,f2.end)
           ||isClose(end,  f2.start) ||  isClose(end,  f2.end));

}

/***********************************************/
int Number::calcAverageX(){
    if(averageX!=0)
        return averageX;

    if(parts.length()==0)
        return 0;

    for(int i=0;i<parts.length();i++)
        averageX += parts[i].averageX();

    averageX = averageX/parts.length();
    return averageX;
}

int Number::calcAverageY(QList<FigurePart> p){
    int averageY=0;
    for(int i=0;i<p.length();i++)
        averageY += p[i].averageY();
    return averageY/p.length();
}

QPoint Number::getBiggestPoint(){
    if(biggestPoint!=QPoint(-1,-1))
        return biggestPoint;
    calcBiggestNode();
    return biggestPoint;
}

bool Number::is8(){
    if(parts.length()!=8)
        return false;
    return calcBiggestNode()==4;
}

int Number::calcBiggestNode(){
    if(biggestNode!=0)
        return biggestNode;
    QHash<QString, int> hash;

    for(int i=0;i<parts.length();i++){//lomaetsa tut
        QString s( QString::number(parts[i].start.x() * 1000 + parts[i].start.y()) ),
                e( QString::number(parts[i].end.x() * 1000 + parts[i].end.y()) );

        hash[s]++;
        hash[e]++;
        if(hash[s]>biggestNode){
            biggestNode=hash[s];
            biggestPoint=parts[i].start;
        }
        if(hash[e]>biggestNode){
            biggestNode=hash[e];
            biggestPoint=parts[i].end;
        }
    }
    return biggestNode;
}

bool Number::is0(){
    if(parts.length()!=8)
        return false;
    QList<FigurePart> copy = parts;
    FigurePart p = copy.takeFirst();
    QPoint _start=p.start,_end=p.end;
    int i=0;

    while(i < copy.length()){
        if(p.isClose(_start,copy[i].start)){
            p = copy.takeAt(i);
            _start=p.end;
            i=0;
        } else if(p.isClose(_start,copy[i].end)){
            p = copy.takeAt(i);
            _start=p.start;
            i=0;
        } else if(p.isClose(_end,copy[i].start)){
            p = copy.takeAt(i);
            _end=p.end;
            i=0;
        } else if(p.isClose(_end,copy[i].end)){
            p = copy.takeAt(i);
            _end=p.start;
            i=0;
        } else {
            i++;
        }
    }

    return (copy.length()==0 && p.isClose(_start,_end));
}

bool Number::is9(){
    return (biggestPoint.x()%50)>25;  
}

bool Number::hasRing(){
    QList<FigurePart> connected,copy;
    copy.append(parts);

    int i=0;
    //get all connected to biggest
    while(i<copy.size()){
        if(copy[i].end==biggestPoint || copy[i].start==biggestPoint){
            connected.append(copy.takeAt(i));
        } else {
            i++;
        }
    }

    //check if every connected - connected to some other part (1 is not)

    int j=0;
    while(j<connected.size()){
        for(int k=0;k<copy.size();k++){
            //if selected part is connected to some other part then it's not lower parts of 1 (lower parts of 1 is connected only to biggestNode)
            if((connected[j].end==biggestPoint && (connected[j].start==copy[k].start || connected[j].start==copy[k].end))||
                    (connected[j].start==biggestPoint && (connected[j].end==copy[k].start || connected[j].end==copy[k].end))){
                connected.removeAt(j);
                j--;
                break;
            }
        }
        j++;
    }

    //if we still have parts in connected list then it's number 1 and it couldn't have ring since 2 parts isn't connected.
    return connected.size()==0;
}


bool Number::is7(){
    double dist=0;

    for(int i=0;i<parts.size();i++)
        dist+=abs(parts[i].calcCurvaturePointDistance());

    return dist<4;
}
bool Number::is5(){
    QList<FigurePart> copy;
    copy.append(parts);
    double dist=0;
    for(int i=0;i<2;i++){
        int top=0;QPoint t(0,255);
        for(int n=0;n<copy.size();n++){
            if(copy[n].start.y()<t.y()){
                top=n;t=copy[n].start;
            }
            if(copy[n].end.y()<t.y()){
                top=n;t=copy[n].end;
            }
        }
        dist+=abs(copy.takeAt(top).calcCurvaturePointDistance())/3.0;
    }
    return dist<1;
}
bool Number::is2(){
    QList<FigurePart> copy;
    copy.append(parts);
    double dist=0;
    for(int i=0;i<2;i++){
        int low=0;QPoint t(0,0);
        for(int n=0;n<copy.size();n++){
            if(copy[n].start.y()>t.y()){
                low=n;t=copy[n].start;
            }
            if(copy[n].end.y()>t.y()){
                low=n;t=copy[n].end;
            }
        }
        dist+=abs(copy.takeAt(low).calcCurvaturePointDistance())/3.0;
    }
    return dist<1;


}

int Number::getNumber(){
    if(num!=-1)
        return num;

    //4 8 0 9 6 1 || 2 5 3 7

    if(is8()){
        num = 8;
    }
    else if(is0()){
        num = 0;
    }
    else if(calcBiggestNode()==3){
        if(hasRing()){
            if(is9())
                num = 9;
            else
                num = 6;
        } else {
            num=1;
        }
    } else if(is7()){
        num = 7;
    } else if(is5()){
        num = 5;
    } else if(is2()){
        num = 2;
    } else {
        num = 3;
    }

    return num;
}
/***********************************************/

breaker::breaker(QObject *parent) :
    QObject(parent)
{

}

void breaker::work(QString coordInfo){
    bool ok;
    figureParts.clear();
    numbers.clear();

    for(int start=0;start<coordInfo.size();start+=12){
        QString partInfo(coordInfo.mid(start,12));
        FigurePart f;
        f.start = QPoint(partInfo.mid(0,2).toInt(&ok,16),partInfo.mid(2,2).toInt(&ok,16));
        if(!ok){
            emit ConsoleLog(QString("<font color=\"red\">cannot convert - ").append(partInfo).append("</font>"));
            return;
        }
        f.end = QPoint(partInfo.mid(4,2).toInt(&ok,16),partInfo.mid(6,2).toInt(&ok,16));
        if(!ok){
            emit ConsoleLog(QString("<font color=\"red\">cannot convert - ").append(partInfo).append("</font>"));
            return;
        }
        f.mid =QPoint(partInfo.mid(8,2).toInt(&ok,16),partInfo.mid(10,2).toInt(&ok,16));
        if(!ok){
            emit ConsoleLog(QString("<font color=\"red\">cannot convert - ").append(partInfo).append("</font>"));
            return;
        }
        figureParts.append(f);
    }


    parseFigures();
    sortFigures();
    check4();
    emit drawFigures();
    emit ConsoleLog(QString("solved - ").append(getNumbers()));
    emit answer(getNumbers());
}

QString breaker::getNumbers(){
    QString s;
    for(int i=0;i<numbers.length();i++)
        s.append(s.number(numbers[i].getNumber()));

    return s;
}

void breaker::check4(){
    if(numbers.length()==5)
        return;

    int n =0;
    while(n<numbers.length()-1){//cause we don't want to check last number
        if((numbers[n].length()==6 && numbers[n+1].length()==2)
             ||(numbers[n+1].length()==6 && numbers[n].length()==2)){
            Number N;
            N.append(numbers[n].parts);
            N.append(numbers[n+1].parts);
            N.num=4;
            numbers.replace(n,N);
            numbers.removeAt(n+1);
        } else {
            n++;
        }

    }

}

void breaker::parseFigures(){
    while(figureParts.length()>0){
        Number number;
        number.append(figureParts.takeFirst());
        int j=0;
        while(j<number.length()){
            int i=0;
            while(i<figureParts.length()){
                if( number.get(j).isClose(figureParts[i]) ){
                      number.append(figureParts.takeAt(i));
                        i--;
                }
                i++;
            }
            j++;
        }
        numbers.append(number);
    }
}

void breaker::sortFigures(){
    QList<Number> sorted;
    while(numbers.length()>0){
        int min= 0;
        for(int n=1;n<numbers.length();n++)
            if(numbers[n].calcAverageX()<numbers[min].calcAverageX())
                min=n;

        sorted.append(numbers.takeAt(min));
    }
    numbers.append(sorted);
}
