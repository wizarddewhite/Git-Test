#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

void Print(unordered_map<string, double>& m, string pre)
{
    unordered_map<string, double>::iterator iter;
    cout << pre << endl;;
    for (iter = m.begin(); iter != m.end(); ++iter)
        cout << "(" << iter->first << ", " << iter->second << ")" << endl;
    cout << endl;
}

int std_type_unordered_map()
{
  unordered_map<string,double> mymap = {
     {"Burger",2.99},
     {"Burger",3.99},
     {"Fries",1.99},
     {"Soda",1.50},
  };

  for (auto& x: {"Burger","Pizza","Salad","Soda"}) {
    if (mymap.count(x)>0)
      cout << "mymap has " << mymap.count(x) << " " << x << endl;
    else
      cout << "mymap has no " << x << endl;
  }

  mymap["Orange"] = 5.05;
  Print(mymap, "Insert Orange:");

  mymap.insert(mymap.end(), pair<string, double>("Cake", 3.88));
  Print(mymap, "Insert Cake at end:");

  mymap.emplace("Onion", 2.38);
  Print(mymap, "Emplace Onion:");

  mymap.at("Onion") = 3.38;
  Print(mymap, "At Onion:");

  unordered_map<string, double>::iterator iter;
  iter = mymap.find("Cake");
  if (iter != mymap.end()) {
     mymap.erase(iter);
     Print(mymap, "Erase Cake:");
  }

  return 0;
}

// https://blog.csdn.net/charles1e/article/details/52042066
class Number
{
    string str;
public:
    Number() { }
    Number(string s) { str = s; }

    const string& get() const
    {
        return str;
    }

    bool operator==(const Number &other) const
    {
       cout << "\tcompare " << get() << " and " << other.get() << endl;
       return (get() == other.get());
    }
};

class Name 
{
    string str;
public:
    Name() {}
    Name(string s) { str = s; }

    const string& get() const
    {
        return str;
    }
};

namespace std{
template <> struct hash<Number>
{
     size_t operator()(const Number& x) const
     {
         cout << "\thashing " << x.get() << endl;
         return std::hash<string>()(x.get());
     }
};
}

void user_defined_unordered_map()
{
    unordered_map<Number, Name> map;
    cout << "emplace 123" << endl;
    map.emplace(Number("123"), Name("John"));
    cout << "emplace 124" << endl;
    map.emplace(Number("124"), Name("Snow"));


    cout << "Looking for 234" << endl;
    unordered_map<Number, Name>::iterator iter;
    iter = map.find(Number("234"));
    if (iter != map.end())
        cout << "Number: " << iter->first.get() << "," << "Name： " << iter->second.get() << endl;
    else
        cout << "Not found!" << endl;
    cout << "Looking for 124" << endl;
    iter = map.find(Number("124"));
    if (iter != map.end())
        cout << "Number: " << iter->first.get() << "," << "Name： " << iter->second.get() << endl;
    else
        cout << "Not found!" << endl;
}

int main()
{
   //std_type_unordered_map();
   user_defined_unordered_map();
   return 0;
}
