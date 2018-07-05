#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

// template function
template <class T>
T const& Max (T const& a, T const& b)
{
	return a < b ? b:a;
}

void function_t()
{
	int i = 39;
	int j = 20;
	cout << "Max(i, j): " << Max<int>(i, j) << endl;

	double f1 = 13.5;
	double f2 = 20.7;
	cout << "Max(f1, f2): " << Max<float>(f1, f2) << endl;

	string s1 = "Hello";
	string s2 = "World";
	cout << "Max(s1, s2): " << Max<string>(s1, s2) << endl;
}

// template class
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

// template class specialization
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

// template function in class
class TF_C {
public:
	template<typename Stream>
	void Serialize(Stream &s) const {
		cout << s << endl;
	}
};

void template_function_in_class()
{
	TF_C tf;
	int a = 1;
	tf.Serialize<int>(a);
	string b = "this is a test";
	tf.Serialize<string>(b);
}

// Non-type parameters for template
template <class T, char N> 
class NTP_TEMP {
public:
	T _m;
	string _name;
	string& get_name() { return _name;}
	NTP_TEMP();
};

template <class T, char N> 
NTP_TEMP<T, N>::NTP_TEMP()
{
	_name = N;
}

void non_type_template()
{
	NTP_TEMP<int, 'i'> nt;
	cout << nt.get_name() << endl;
}

int main()
{
	//function_t();
	//class_t();
	//class_template_specialization();
	template_function_in_class();
	//non_type_template();
	return 0;
}
