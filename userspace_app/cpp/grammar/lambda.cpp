#include <iostream>
#include <string>
#include <ctime>

using namespace std;

void implicit_return()
{
	auto lambda = [](float f) { return f; };
	cout << lambda(-3.5) << endl;
}

void obvious_return()
{
	auto lambda = [](float f) -> float { return f; };
	cout << lambda(5.5) << endl;
}

void capture_value()
{
	float v = 6;
	auto lambda = [=](float f) mutable { v += f; return v; };

	cout << lambda(v) << endl;
	cout << v << endl;
}

void capture_ref()
{
	float v = 6;
	auto lambda = [&](float f) { v += f; return v; };

	cout << lambda(v) << endl;
	cout << v << endl;
}

int main()
{
	// implicit_return();
	// obvious_return();
	capture_value();
	// capture_ref();
}
