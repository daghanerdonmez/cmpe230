//CURRENCY MYCLASS.CPP
#include <QtGui>
#include "myclass.h"
#include <QRegExp>

MyClass::MyClass(QWidget *parent) : QLabel(parent)
{

     label = new QLabel("Connecting");
     QHBoxLayout *layout = new QHBoxLayout;

     layout->addWidget(label);
     setLayout(layout);

     manager = new QNetworkAccessManager(this) ; 

     connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(replyFinished(QNetworkReply *)));

     // Initiate retrieval of  the data from the web page.
     // When completed, slot replyFinished will be called. 
     manager->get(QNetworkRequest(QUrl("https://www.bitstamp.net/api/v2/ticker/btcusd")));
}

void MyClass::replyFinished(QNetworkReply *reply)  {

     QString str ; 
     int pos = 0;

     // read the data fetched from the web site 
     QString data = (QString) reply->readAll();

     // use pattern matching to extract the rate 
     QRegExp rx("\"last\": \"(\\d+\\.\\d+)\"");

     if ( rx.indexIn(data, pos) != -1 ) {
       str = QString("BTC/USD:  ") + rx.cap(1);    // rate found 
     }
     else {
       str = QString("Error") ; 
     }

     // set the text of the label 
     label->setText(str) ;
}
