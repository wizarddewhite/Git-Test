#include <iostream>
#include <sstream>
#include <algorithm>

// g++ -std=c++11 stringstream.cpp

using namespace std;
int main(int argc,char **argv)
{
	stringstream m_sstream;
	string result;
	int i=1000;
	m_sstream<<i;
	m_sstream>>result;
	cout << "output m_sstream: " << m_sstream << endl;;
	cout << "convert to string: " << result << endl;;
	cout << "sizeof(m_sstream): " << sizeof(m_sstream) << endl;;

	m_sstream.clear();
	int port;
	m_sstream << "6666666666";
	m_sstream >> port;
	cout << "output m_sstream: " << m_sstream << endl;;
	cout << "convert to int: " << port << endl;;
	cout << "sizeof(m_sstream): " << sizeof(m_sstream) << endl;;
	return 0;
}
