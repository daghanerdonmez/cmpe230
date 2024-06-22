#include <iostream>
#include <string>

using namespace std;

struct MyRec {
   string myname ; 
   int myage ; 
   
   void printmyinfo() {
      cout << myname << "," << myage << endl ; 
   }   
} ; 

int  main(int argc, char *argv[]) 
{
  MyRec  mr = {"Joe",34} ; 

  mr.printmyinfo() ; 

  mr.myname = "ali" ; 
  mr.myage = 44 ; 
  mr.printmyinfo() ; 

  return(0) ; 
}
