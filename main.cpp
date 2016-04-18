#include <iostream>
#include <string>
#include <tuple>
#include <kapok/Kapok.hpp>

#include "router.hpp"
#include "client_proxy.hpp"

struct person
{
	int age;
	std::string name;

	META(age, name);
};

void add(int a, int b)
{
	std::cout << a + b << std::endl;
}
void hello()
{
	std::cout << "hello" << std::endl;
}

void test_one(int d)
{
	std::cout << d << std::endl;
}

void foo(std::string b, int a)
{
	std::cout << b << std::endl;
}

void fun(const person& ps)
{
	std::cout << ps.name << std::endl;
}

void fun1(const person& ps, int a)
{
	std::cout << ps.name << std::endl;
}

void register_handler()
{

}

int main()
{
	using namespace std;

	person _person = { 20, "aa" };

	router r;
	//����handler
	r.register_handler("fun1", &fun1);

	r.register_handler("fun", &fun);
	r.register_handler("add", &add);
	r.register_handler("about", &hello);
	r.register_handler("foo", &foo);
	r.register_handler("test_one", &test_one);
	
	try
	{
		//��������
		client_proxy client(r);
		client.call("about");
		client.call("test_one", 2);
		
		person p = { 20, "aa" };
		client.call("fun", p);
		client.call("fun1", p, 1);

		client.call("foo", "test", 1);
	}
	catch (std::runtime_error &error)
	{
		std::cerr << error.what() << std::endl;
	}

	return 0;
}

