#include <boost/thread/thread.hpp>
#include <string>
#include <ctime>

// g++ boost_thread.cpp -std=c++11 -Iopt/boost_1_57_0/include/ -L opt/boost_1_57_0/lib/ -lboost_system -lboost_wave -lboost_iostreams -lboost_thread
// LD_LIBRARY_PATH="opt/boost_1_57_0/lib/" ./a.out
// curl localhost:2014

void thread_handle()
{
        static int num = 0;
        std::cout << num++ << std::endl;
        sleep(10);
}

class T
{
	int _a;
public:
	void assign(int a)
	{
		_a = a;
		std::cout << "_a is assigned to " << _a << std::endl;
	}
};

int main()
{
        boost::thread t(thread_handle);
        t.join();

	T c;
	boost::thread ct(boost::bind(&T::assign, &c, 5));
	ct.join();

        boost::thread_group tg;
        int i;
        for (i = 0; i < 10; i++)
                tg.create_thread(thread_handle);
	tg.create_thread(boost::bind(&T::assign, &c, 5));
        tg.join_all();
}

