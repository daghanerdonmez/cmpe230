#include <iostream>
using namespace std;

int addition(int x, int y) {
  return(x+y) ;
}

double addition(double x, double y) {
  return(x+y) ;
}

int main() {
  
  cout << addition(1,2) << endl ; 
  cout << addition(1.8,2.9) << endl ; 

  return(0) ;
}
