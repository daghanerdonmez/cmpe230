#include <iostream>
#include <string>
using namespace std;

template <class T>
T addition(T x, T y) {
  return(x+y) ;
}


int main() {
  
  cout << addition<int>(1,2) << endl ; 
  cout << addition<double>(1.8,2.9) << endl ; 
  cout << addition<string>("hello"," world") << endl ; 

  return(0) ;
}
