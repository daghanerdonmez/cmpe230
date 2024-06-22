//CURRENCY MAIN.CPP
#include <QtCore>
#include <QApplication>
#include "myclass.h"

int main(int argc,char *argv[])
{
   QApplication a(argc, argv); 
   MyClass my ; 

   my.show(); 

   return a.exec();
}


