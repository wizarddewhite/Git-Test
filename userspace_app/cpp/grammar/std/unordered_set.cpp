#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;

void std_unordered_set()
{
    unordered_set<int>s;
    s.insert(4);
    s.insert(2);
    s.insert(3);
    s.insert(1);
    for(auto it=s.begin();it!=s.end();++it)
        cout<<*it<<" ";
    cout<<endl;
}

class Student {
public:
    Student() {}
    Student(string _name, int _age, int _height)
	    :name(_name), age(_age),height(_height) {}
    string name;
    int age;
    int height;

    bool operator==(const Student &other) const
    {
       cout << "\tcompare " << name << " and " << other.name << endl;
       return (name == other.name);
    }
};

namespace std{
template <> struct hash<Student>
{
     size_t operator()(const Student& x) const
     {
         cout << "\thashing " << x.name << endl;
         return std::hash<string>()(x.name);
     }
};
}

void user_defined_unordered_set()
{
    unordered_set<Student> set;
    set.insert(Student("Rachel", 22, 18));
    set.insert(Student("Joy", 25, 22));

    for(auto it=set.begin();it!=set.end();++it)
        cout << it->name << endl;;
    cout<<endl;
}

int main()
{
   //std_unordered_set();
   user_defined_unordered_set();
   return 0;
}
