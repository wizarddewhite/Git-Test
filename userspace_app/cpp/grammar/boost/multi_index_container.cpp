#include <string>
#include <iostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

using namespace boost;
using namespace boost::multi_index;
using namespace std;
struct Employee{
    int id;
    string name;
    int age;

    Employee(int id_,string name_,int age_):id(id_),name(name_),age(age_){}

    friend ostream& operator<<(ostream& os,const Employee& e)
    {
        os<<e.id<<" "<<e.name<<" "<<e.age<<endl;
        return os;
    }

    std::size_t name_length()const{return name.size();}
};

struct by_name;
struct reverse_id;

typedef multi_index_container<
Employee,
   indexed_by<
      ordered_unique<member<Employee, int, &Employee::id> >,
      ordered_non_unique<member<Employee, string, &Employee::name> >,
      ordered_non_unique<member<Employee, int, &Employee::age> >,
      ordered_non_unique<const_mem_fun<Employee, std::size_t, &Employee::name_length> >,
      ordered_non_unique<tag<by_name>, member<Employee, string, &Employee::name> >,
      ordered_unique<tag<reverse_id>, member<Employee, int, &Employee::id>, std::greater<int> >
   >
> EmployeeContainer;

typedef EmployeeContainer::nth_index<0>::type IdIndex;
typedef EmployeeContainer::nth_index<1>::type NameIndex;
typedef EmployeeContainer::nth_index<2>::type AgeIndex;
typedef EmployeeContainer::nth_index<3>::type NameLenIndex;

int main(){
    EmployeeContainer con;
    con.insert(Employee(0,"Joe",31));
    con.insert(Employee(1,"Robert",27));
    con.insert(Employee(2,"John",40));
    con.insert(Employee(3,"John",25));
    con.insert(Employee(4,"Zoe",25));

    IdIndex& ids = con.get<0>();
    copy(ids.begin(),ids.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    auto& rev_ids = con.get<reverse_id>();
    copy(rev_ids.begin(),rev_ids.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    NameIndex& names = con.get<1>();
    copy(names.begin(), names.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    NameLenIndex& namelen = con.get<3>();
    copy(namelen.begin(), namelen.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    //  names.erase(names.begin());

    AgeIndex& ages = con.get<2>();
    copy(ages.begin(), ages.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    auto& name_ids = con.get<by_name>();
    copy(name_ids.begin(),name_ids.end(), ostream_iterator<Employee>(cout));
    cout << endl;

    // find
    NameIndex::iterator name_it = names.find("Robert");
    if (name_it != names.end())
        cout << "Name: " << name_it->name << " Age: "<< name_it->age << endl;
    else
        cout << "No Robert" << endl;

    name_it = names.find("John");
    if (name_it != names.end()) {
        int count = names.count("John");
        for (; count > 0; count--, name_it++) {
            cout << "Name: " << name_it->name << " Age: "<< name_it->age << endl;
        }
    } else {
        cout << "No John" << endl;
    }
    return 0;
}
