#include <iostream>
using namespace std;

void not_catch()
{
	throw 6;
}

int main()
{
	try
	{
		throw 20;
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
	
	not_catch();
	cout << "normal exit" << endl;
	return 0;
}
