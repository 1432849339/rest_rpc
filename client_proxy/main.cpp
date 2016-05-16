#include <iostream>
#include <thread>
#include <chrono>
#include <kapok/Kapok.hpp>
#include "client_proxy.hpp"


//resultҪô�ǻ������ͣ�Ҫô�ǽṹ�壻������ɹ�ʱ��codeΪ0, ����������޷������͵ģ���resultΪ��; 
//������з���ֵ�ģ���resultΪ����ֵ��response_msg�����л�Ϊһ����׼��json�����ط����ͻ��ˡ� 
//������Ϣ�ĸ�ʽ��length+body����4���ֽڵĳ�����Ϣ������ָʾ����ĳ��ȣ��Ӱ�����ɡ� 
template<typename T>
struct response_msg
{
	int code;
	T result; //json��ʽ�ַ������������ͻ����ǽṹ��.
	META(code, result);
};

enum result_code
{
	OK = 0,
	FAIL = 1,
	EXCEPTION = 2,

};

struct person
{
	int age;
	std::string name;

	META(age, name);
};

void test_client()
{
	try
	{
		boost::asio::io_service io_service;
		DeSerializer dr;
		client_proxy client(io_service);
		client.connect("192.168.2.154", "9000");
		person p = { 20, "aa" };
		//auto str = client.make_json("fun1", p, 1);
		//client.call(str);
		
		std::string result = client.call("fun1", p, 1);
		dr.Parse(result);

		response_msg<int> response = {};
		dr.Deserialize(response);
		if (response.code == result_code::OK)
		{
			std::cout << response.result << std::endl;
		}
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_async_client()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.async_connect("127.0.0.1", "9000", [&client] (boost::system::error_code& ec)
		{
			if (ec)
			{
				std::cout << "connect error." << std::endl;
				return;
			}

			client.async_call("add", [&client](boost::system::error_code ec, std::string result)
			{
				if (ec)
				{
					std::cout << "call error." << std::endl;
					return;
				}

				DeSerializer dr;
				dr.Parse(result);

				response_msg<int> response = {};
				dr.Deserialize(response);
				if (response.code == result_code::OK)
				{
					std::cout << response.result << std::endl;
				}

			},
				1, 2);

		});

		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_spawn_client()
{
	try
	{
		boost::asio::io_service io_service;
		boost::asio::spawn(io_service, [&io_service] (boost::asio::yield_context yield)
		{
			DeSerializer dr;
			client_proxy client(io_service);
			client.async_connect("127.0.0.1", "9000", yield);
			//auto str = client.make_json("fun1", p, 1);
			//client.call(str);

			std::string result = client.async_call("add", yield, 1,2);
			dr.Parse(result);

			response_msg<int> response = {};
			dr.Deserialize(response);
			if (response.code == result_code::OK)
			{
				std::cout << response.result << std::endl;
			}
		});
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_performance()
{
	try
	{
		boost::asio::io_service io_service;
		
		client_proxy client(io_service);
		client.connect("192.168.2.154", "9000");

		auto str = client.make_json("add", 1, 2);
		std::thread thd([&io_service] {io_service.run(); });
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		while (true)
		{
			client.call(str);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

int main()
{
	//test_performance();
	//test_client();
	test_async_client();
	test_spawn_client();
	return 0;
}