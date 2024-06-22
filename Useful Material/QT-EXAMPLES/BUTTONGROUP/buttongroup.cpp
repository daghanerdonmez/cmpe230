#include <QApplication>
#include <QPushButton>
#include <QButtonGroup>
#include <QLCDNumber>
#include <QHBoxLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	
    QWidget       window ; 
    QButtonGroup  buttongroup ; 
    QPushButton  *button[2]  ;
    QHBoxLayout   layout ;
    QLCDNumber    disp ; 
    QString s ;

    for(int i=0 ; i < 2 ; i++) {
      s = QString::number(i+1) ; 
      button[i] = new QPushButton(s);
      layout.addWidget(button[i]);
      buttongroup.addButton(button[i],i+1) ; 
    }
    QObject::connect(&buttongroup, SIGNAL(buttonClicked(int)),
                     &disp, SLOT(display(int))); 

    layout.addWidget(&disp);

    window.setLayout(&layout);
    window.show();
	
    return app.exec();
}
