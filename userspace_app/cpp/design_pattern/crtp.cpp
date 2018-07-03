// CRTP: Curiously Recurring Template Pattern
#include <iostream>

using namespace std;

template <class Derived>
class Base
{
public:
	// access Derived class function from Base
	void print()
	{
		auto* derived = static_cast<Derived*>(this);
		derived->do_print();
	}
};

class Derived1 : public Base<Derived1>
{
public:
	void do_print()
	{
		cout << "Derived1" << endl;
	}
};

int main()
{
	Derived1 d;
	d.print();
	return 0;
}
