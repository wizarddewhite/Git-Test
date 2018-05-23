#include <memory>
#include <iostream>

using namespace std;

struct Base
{
    Base() { cout << "Base::Base()" << endl; }
    Base(string n):name(n) { cout << "Base::Base(" << name << ")" << endl; }
    ~Base() { cout << "Base::~Base(" << name << ")" << endl; }
    virtual void print() {cout << "  Base::print(" << name << ")" << endl;}

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
	auto_pointer();
	//shared_pointer();
	//unique_pointer();
	return 0;
}
