#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp> 
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

void func(int x, int y) 
{
	cout << x << ", " << y << endl;
}

void func_param()
{
	boost::bind(&func, 3, 4)();
	boost::bind(&func, 3, _1)(4);
	boost::bind(&func, _1, _2)(3, 4);
}

int main()
{
	func_param();
}
