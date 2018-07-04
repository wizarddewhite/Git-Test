#include <iostream>  

// g++ template_variadic.cpp
  
using namespace std;  
  
template <typename... Ts>
void print(Ts ... args);

template <>
void print<>() {  
    cout << endl;  
}  

template <typename First, typename... Rest>
void print(const First& first, const Rest&... rest) {  
    cout << first << ", ";  
    print(rest...); // recursive call using pack expansion syntax  
}  
  
int main()  
{  
    print(); // calls first overload, outputting only a newline  
    print(1); // calls second overload  
  
    // these call the third overload, the variadic template,   
    // which uses recursion as needed.  
    print(10, 20);  
    print(100, 200, 300);  
    print("first", 2, "third", 3.14159);  
}  
