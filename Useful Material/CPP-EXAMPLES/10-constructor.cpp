// Example from: https://www.w3schools.com/CPP/cpp_constructors.asp   
#include <iostream>
using namespace std;

class MyClass {     // The class
  public:           // Access specifier
    MyClass() {     // Constructor
      cout << "Hello World!" << endl ;
    }
    ~MyClass() {     // Constructor
      cout << "Bye " << endl ;
    }
};

int main() {
  MyClass myObj;    // Create an object of MyClass (this will call the constructor)
  MyClass *myObj2;  

  myObj2 = new MyClass() ; 

  cout << "here" << endl  ; 
  delete(myObj2) ; 
  return 0;
}

