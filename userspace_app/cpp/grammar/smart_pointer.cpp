#include <memory>
#include <iostream>

using namespace std;

struct Base : public enable_shared_from_this<Base>
{
    Base() { cout << "Base::Base()" << endl; }
    Base(string n):name(n) { cout << "Base::Base(" << name << ")" << endl; }
    ~Base() { cout << "Base::~Base(" << name << ")" << endl; }
    virtual void print() {cout << "  Base::print(" << name << ")" << endl;}

    shared_ptr<Base> get_self()
    {
	    return shared_from_this();
    }

    string name;
};

void raw_pointer()
{
	Base *d = new Base("raw pointer");
	d->print();

	// must release the memory by delete
	delete d;
}

// deprecated
void auto_pointer()
{
	auto_ptr<Base> sd(new Base("auto pointer"));
	// automatically released

	auto_ptr<Base> csd;
	// ownership transferred
	csd = sd;
	//sd->print(); // will crash
	csd->print();
	cout << " sd addr:" << sd.get() << endl;
	cout << "csd addr:" << csd.get() << endl;
}

void func_param_shared_ptr(const std::shared_ptr<Base>& b)
{
	cout << "  b count" << b.use_count() << endl;
	cout << "  b addr:" << b.get() << endl;
}

void shared_pointer()
{
	shared_ptr<Base> sd(new Base("shared pointer1"));
	// same as
	// std::shared_ptr<Base> sd = std::make_shared<Base> ("shared pointer1");
	// automatically released

	shared_ptr<Base> csd;
	cout << " sd count" << sd.use_count() << endl;
	func_param_shared_ptr(sd);

	csd = sd;
	cout << " sd count" << sd.use_count() << endl;
	cout << "csd count" << sd.use_count() << endl;
	sd->print();
	csd->print();
	cout << " sd addr:" << sd.get() << endl;
	cout << "csd addr:" << csd.get() << endl;
}

void shared_pointer_from_ptr()
{
	Base *pb = new Base("shared ptr from pointer");
	cout << "addr  of pb:" << pb << endl;

	// works fine, the Base will be released after function return
	shared_ptr<Base> p1(pb);
	cout << "addr  of p1:" << p1.get() << endl;
	cout << "count of p1:" << p1.use_count() << endl;

	// fail, since p2 not aware p1, then Base will be released twice
	// shared_ptr<Base> p2(pb);
	// cout << "addr  of p2:" << p2.get() << endl;
	// cout << "count of p2:" << p2.use_count() << endl;

	// success by enable_shared_from_this
	shared_ptr<Base> p3 (pb->get_self());
	cout << "addr  of p3:" << p3.get() << endl;
	cout << "count of p3:" << p3.use_count() << endl;
}

void unique_pointer()
{
	unique_ptr<Base> sd(new Base("unique pointer"));
	// automatically released

	unique_ptr<Base> csd;
	// not allowed
	//csd = sd;

	// ownership transferred
	csd = move(sd);
	//sd->print(); // will crash
	csd->print();
	cout << " sd addr:" << sd.get() << endl;
	cout << "csd addr:" << csd.get() << endl;
}

int main()
{
	//raw_pointer();
	//auto_pointer();
	//shared_pointer();
	//unique_pointer();
	shared_pointer_from_ptr();
	return 0;
}
