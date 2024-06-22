#include <iostream>
#include <string>

using namespace std;

string a    = "drink";
string & b  = a ; 

void f(int & d) {
    d = 8 ; 
}

int  main(int argc, char *argv[]) 
{
  int c = 4 ; 

  cout << a << " " << b << endl ; 

  f(c) ; 

  cout << c << endl ; 

  return(0) ; 
}
