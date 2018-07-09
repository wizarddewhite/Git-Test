#include <iostream>

using namespace std;

void Func(int i = 1, float f = 2.0f, double d = 3.0)
{
    cout << i << ", " << f << ", " << d << endl ;
}

int main()
{
	Func();
	Func(10);
	return 0;
}
