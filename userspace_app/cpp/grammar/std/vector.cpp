#include <iostream>
#include <vector>
#include <algorithm>

// g++ -std=c++11 vector.cpp

using namespace std;
int main(int argc,char **argv)
{
	// define vector<int>
	vector<int> vi={10,20,30};
	auto it=vi.begin();
	it++;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
	cout << endl;

	// it points to 2nd place
	vi.emplace(it,999);
	cout<<endl<<"after emplace(it,999)"<<endl;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
	// emplace_back
	vi.emplace_back(888);
	cout<<endl<<"after emplace_back(888)"<<endl;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
	cout << endl;

	// resize
	vi.resize(10);
	cout<<endl<<"after resize(10)"<<endl;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
	vi.resize(3);
	cout<<endl<<"after resize(3)"<<endl;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
	cout << endl;

	// count
	cout << endl << "size of this vector is: " ;
	cout << vi.size() << endl;

	// data
	auto* data = vi.data();
	cout << endl << "Underlying address: " << data << endl;
	cout << "Access content via pointer:" << *(data) << " "<< *(data+1) << endl;
	
	cout<<endl;	
	vi.clear();
	cout << endl << "after clear size of this vector is: " ;
	cout << vi.size() << endl;
}
