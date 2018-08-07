#include <string>
#include <memory>
#include <iostream>

// g++ --std=c++11 shared_ptr.cpp

class Good: public std::enable_shared_from_this<Good>
{
public:
    std::shared_ptr<Good> getptr() {
        return shared_from_this();
    }
};
 
int main()
{
	// Good: the two shared_ptr's share the same object
	std::shared_ptr<Good> gp1 = std::make_shared<Good>();
	std::shared_ptr<Good> gp2 = gp1->getptr();
	std::cout << "gp2.use_count() = " << gp2.use_count() << '\n';
	return 0;
}

