#include <iostream>

// g++ -std=c++11 union_template.cpp

using namespace std;

template<typename X, typename... Ts>
struct Position;

template<typename X>
struct Position<X> {
    static const int pos = -1;
};

template<typename X, typename... Ts>
struct Position<X, X, Ts...> {
    static const int pos = 0;
};

template<typename X, typename T, typename... Ts>
struct Position<X, T, Ts...> {
    static const int pos = Position<X, Ts...>::pos != -1 ? Position<X, Ts...>::pos + 1 : -1;
};

template<typename... Ts>
struct Type_Info;

template<typename T, typename... Ts>
struct Type_Info<T&, Ts...> {
    static const bool no_reference_types = false;
    static const size_t size = Type_Info<Ts...>::size > sizeof(T&) ? Type_Info<Ts...>::size : sizeof(T&);
    static const size_t count = 1 + Type_Info<Ts...>::count;
};

template<typename T, typename... Ts>
struct Type_Info<T, Ts...> {
    static const bool no_reference_types = Type_Info<Ts...>::no_reference_types;
    static const size_t size = Type_Info<Ts...>::size > sizeof(T) ? Type_Info<Ts...>::size : sizeof(T&);
    static const size_t count = 1 + Type_Info<Ts...>::count;
};

template<>
struct Type_Info<> {
    static const bool no_reference_types = true;
    static const size_t count = 0;
    static const size_t size = 0;
};

int main()
{
	// sizeof(Type_Info<>) is the largest size of all the type
	cout << Type_Info<int, char>::size << endl;
	cout << Type_Info<int, char, double>::size << endl;

	// pos stands for the position of the first type in following list of
	// types
	cout << Position<int, int>::pos << endl;
	cout << Position<double, int, char, double>::pos << endl;
	return 0;
}
