#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
 
using namespace std;

class A
{
public:
	int a;

	// constructor
	A();
	A(int _a);

	// copy constructor
	A(const A &oa);

	void operator()();
	void operator()(int a);
};

// constructor with no parameter
A::A()
{
	cout << "A::A()" << endl; 
}

// constructor with parameter
A::A(int _a):a(_a)
{
	cout << "A::A(int)" << endl; 
}

// copy constructor
A::A(const A &oa)
{
	cout << "A::A(const A&)" << endl;
	a = oa.a;
}

// rewrite operator()
void A::operator()()
{
	cout << "A::operator()" << endl; 
}

// rewrite operator()
void A::operator()(int a)
{
	cout << "A::operator(int, int)" << endl; 
}

void constructor_test()
{
	A a1;
	// compile error
	// A a2();
	// works
	A a3 = A();
	A a4(1);
	A a5 = A(2);

	// copy constructor
	A a6(a5);
	A a7 = a6;
}

void operator_test()
{
	A a;
	// apply to an object instead of class
	a();
	a(1);
}

void test(A s)
{
	cout << s.a << endl;
}

void function_test()
{
	// parameter is passed by value
	A a;
	test(a); // copy once

	// parameter is created through constructor
	// compile error
	// test(A);
	test(A()); // no copy
	test(A(2));
	test(4);
}

void test_ref(A& s)
{
	cout << s.a << endl;
}

void function_test_ref()
{
	// parameter is passed by value
	A a;
	test_ref(a); // no copy
}

int main()
{
	// constructor_test();
	// operator_test();
	// function_test();
	function_test_ref();
}
