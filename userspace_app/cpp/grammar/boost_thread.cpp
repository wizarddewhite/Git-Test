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

int main()
{
	boost::thread t(thread_handle);
	t.join();

	boost::thread_group tg;
	int i;
	for (i = 0; i < 10; i++)
		tg.create_thread(thread_handle);
	tg.join_all();
}

