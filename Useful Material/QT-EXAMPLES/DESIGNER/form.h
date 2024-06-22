#include <QWidget>
#include "ui_form.h"

class Form : public QWidget
{
  Q_OBJECT

  public:
    Form() ;

  private slots:
    void myadditem() ;
    void mydeleteitem() ;

private:
    Ui::Form ui ;

} ; 
