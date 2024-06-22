#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
     int rc ; 
     QApplication app(argc, argv);
     QLabel label("Hello Qt!");

     label.show();

     rc = app.exec();
	 return(rc) ;
}
