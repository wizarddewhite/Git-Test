#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp> 
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

void cat()
{
	int ab = 9;
	int xyz = 19;
	cout << BOOST_PP_CAT(a, b) << endl;
	cout << BOOST_PP_CAT(x, BOOST_PP_CAT(y, z)) << endl;
}

#define DECLARE_VAR(r, data, name) \
	int BOOST_PP_CAT(local_, name);

void seq_for_each()
{
	BOOST_PP_SEQ_FOR_EACH(DECLARE_VAR, unused, (a) (b))
	local_a = 123;
	local_b = 456;

	cout << local_a << endl;
	cout << local_b << endl;
}

int main()
{
	seq_for_each();
	return 0;
}
