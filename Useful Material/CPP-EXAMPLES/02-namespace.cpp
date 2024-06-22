#include <iostream>
using namespace std;

namespace mynames
{
  int x = 5 ;
  int y = 10 ;
}

namespace yournames
{
  int x = 1 ;
  int y = 2 ;
}

int main () {
  using namespace mynames ;

  cout << x << endl;
  cout << y << endl;

  cout << yournames::x << endl;
  cout << yournames::y << endl;

  return 0;
}
