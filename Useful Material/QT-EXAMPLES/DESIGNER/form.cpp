#include <QWidget>
#include "form.h"

Form::Form() : QWidget()
{
   ui.setupUi(this) ;

   ui.addbutton->setDefault(true);
   connect(ui.addbutton,SIGNAL(clicked()),this,SLOT(myadditem())) ;
   connect(ui.deletebutton,SIGNAL(clicked()),this,SLOT(mydeleteitem())) ;
   connect(ui.closebutton,SIGNAL(clicked()),this,SLOT(close())) ;
   connect(ui.line,SIGNAL(returnPressed()),this,SLOT(myadditem())) ;
}

void Form::myadditem()
{
   QString s ;
   
   s = ui.line->text() ; 
   if ( s.size() > 0) {
     ui.wordlist->addItem(s) ; 
	 ui.line->setText("") ;
   }
}


void Form::mydeleteitem()
{
   delete(ui.wordlist->currentItem());
}
