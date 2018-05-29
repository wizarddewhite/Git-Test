#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

template <class T>
T const& Max (T const& a, T const& b)
{
	return a < b ? b:a;
}

void function_t()
{
	int i = 39;
	int j = 20;
	cout << "Max(i, j): " << Max(i, j) << endl;

	double f1 = 13.5;
	double f2 = 20.7;
	cout << "Max(f1, f2): " << Max(f1, f2) << endl;

	string s1 = "Hello";
	string s2 = "World";
	cout << "Max(s1, s2): " << Max(s1, s2) << endl;
}

template <class T>
class Stack {
private:
	vector<T> elems;     

public:
	void push(T const&);
	void pop();
	T top() const;
	bool empty() const{
		return elems.empty();
	}
};

template <class T>
void Stack<T>::push (T const& elem)
{
	elems.push_back(elem);
}

template <class T>
void Stack<T>::pop ()
{
	if (elems.empty()) {
		throw out_of_range("Stack<>::pop(): empty stack");
	}
	elems.pop_back();
}

template <class T>
T Stack<T>::top () const
{
	if (elems.empty()) {
		throw out_of_range("Stack<>::top(): empty stack");
	}
	return elems.back();
}

void class_t()
{
	try {
		Stack<int>    intStack;  
		Stack<string> stringStack;

		intStack.push(7);
		cout << intStack.top() <<endl;

		stringStack.push("hello");
		cout << stringStack.top() << std::endl;
		stringStack.pop();
		stringStack.pop();
	}
	catch (exception const& ex) {
		cerr << "Exception: " << ex.what() <<endl;
		return;
	}
}

template <>
class Stack<float> {
private:
	vector<float> elems;     

public:
	void push(float const&);
	void pop();
	float top() const;
	bool empty() const{
		return elems.empty();
	}
};

void Stack<float>::push (float const& elem)
{
	elems.push_back(elem);
}

void Stack<float>::pop ()
{
	if (elems.empty()) {
		throw out_of_range("Stack<float>::pop(): empty stack");
	}
	elems.pop_back();
}

float Stack<float>::top () const
{
	if (elems.empty()) {
		throw out_of_range("Stack<float>::top(): empty stack");
	}
	return elems.back();
}

void class_template_specialization()
{
	try {
		Stack<float>    floatStack;  

		floatStack.push(7);
		cout << floatStack.top() <<endl;
		floatStack.pop();
		cout << floatStack.top() <<endl;
	}
	catch (exception const& ex) {
		cerr << "Exception: " << ex.what() <<endl;
		return;
	}
}
int main()
{
	//function_t();
	//class_t();
	class_template_specialization();
	return 0;
}
