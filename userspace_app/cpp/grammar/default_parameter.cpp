#include <iostream>

using namespace std;

struct A {
	A(int _a):a(_a){}
	int a = 18;
};

struct B {
	A a = A(3);
	int b = 9;
};

void Func(int i = 1, float f = 2.0f, double d = 3.0)
{
    cout << i << ", " << f << ", " << d << endl ;
}

int main()
{
	Func();
	Func(10);

	B b;
	cout << b.a.a << endl;
	cout << b.b << endl;
	return 0;
}
