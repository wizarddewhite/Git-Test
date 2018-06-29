#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp> 
#include <string>
#include <iostream>
#include <ctime>

// g++ boost_bind.cpp -std=c++11 -Iopt/boost_1_57_0/include/ -L opt/boost_1_57_0/lib/ -lboost_system -lboost_wave -lboost_iostreams
// LD_LIBRARY_PATH="opt/boost_1_57_0/lib/" ./a.out
// curl localhost:2014

using namespace std;

class T
{
	int _a;
public:
	void assign(int a)
	{
		_a = a;
		cout << "_a is assigned to " << _a << endl;
	}
};

void func(int x, int y) 
{
	cout << x << ", " << y << endl;
}

void func_param()
{
	boost::bind(&func, 3, 4)();
	boost::bind(&func, 3, _1)(4);
	boost::bind(&func, _1, _2)(3, 4);
	boost::bind(&func, _2, _1)(3, 4);

	T t;
	boost::bind(&T::assign, &t, 5)();
	boost::bind(&T::assign, &t, _1)(7);
}

int main()
{
	func_param();
}
