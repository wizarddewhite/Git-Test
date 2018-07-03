#include <iostream>

// g++ -std=c++11 union_template.cpp

using namespace std;

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
	cout << Type_Info<int, char>::size << endl;
	cout << Type_Info<int, char, double>::size << endl;
	return 0;
}
