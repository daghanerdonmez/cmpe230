#include <QApplication>
#include <QPushButton>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QTranslator appTranslator;
        QString t = QLocale::system().name() ;
	
	t = "linguist_" + t ; 
	appTranslator.load(t) ; 
	app.installTranslator(&appTranslator);
	
    QPushButton *button = new QPushButton(QObject::tr("Hello"));
    QObject::connect(button, SIGNAL(clicked()),
                     &app, SLOT(quit()));
    button->show();
	
    return app.exec();
}
