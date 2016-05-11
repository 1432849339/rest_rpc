#include <iostream>
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
		client_proxy client(io_service, "127.0.0.1", "9000");
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

int main()
{
	test_client();
	return 0;
}