//  example from:  https://www.w3schools.com/CPP/cpp_encapsulation.asp  
#include <iostream>
using namespace std;

class Employee {
  private:
    // Private attribute
    int salary;

  public:
    // Setter
    void setSalary(int s) {
      salary = s;
    }
    // Getter
    int getSalary() {
      return salary;
    }
};

int main() {

  Employee myObj;
  myObj.setSalary(50000);
  cout << myObj.getSalary();
  // cout << myObj.salary ;   // cannot access private 
  return 0;

}
