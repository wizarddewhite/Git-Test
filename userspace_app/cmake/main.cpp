#include <iostream>
#include <string>
#include "Test.h"

using namespace std;

int main(int argc, char *argv[])
{
	string a, b;
	if (argc != 3)
		return 0;

	a = string(argv[1]);
	b = string(argv[2]);

	cout << add(stoi(a),stoi(b)) << endl;

	return 0;
}
