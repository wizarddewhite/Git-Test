#include <deque>
#include <string>
#include <iostream>
 
struct President
{
    std::string name;
    std::string country;
    int year;
 
    President(): name("null"), country("world"), year(0) {}
    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed.\n";
    }
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved.\n";
    }
    President& operator=(const President& other) = default;
};
 
int main()
{
    std::deque<President> elections;
    std::cout << "emplace_back:\n";
    elections.emplace_back("Nelson Mandela", "South Africa", 1994);
    elections.emplace_back("George W. Bush", "America", 2001);
    elections.emplace_back("William Jefferson Clinton", "America", 1992);
    elections.emplace_back();
 
    std::cout << "\nContents:\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }

    elections.pop_back();

    std::cout << std::endl;
    std::cout << "\nContents after pop_back():\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }

    std::cout << std::endl;
    if (elections.empty())
        std::cout << "elections is empty" << std::endl;
    else
        std::cout << "elections has " << elections.size() << " presidents" << std::endl;

    President const& p = elections.back();
    std::cout << "Last president's name :" << p.name << std::endl;
}

