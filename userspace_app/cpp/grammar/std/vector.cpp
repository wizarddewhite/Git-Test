#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main(int argc,char **argv)
{
	// define vector<int>
	vector<int> vi={10,20,30};
	auto it=vi.begin();
	it++;
	cout<<"now vi is :";
	for_each(vi.begin(),vi.end(),[](int m){cout<<m<<" ";});
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
	
	cout<<endl;	
}
