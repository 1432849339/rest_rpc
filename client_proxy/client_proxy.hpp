#pragma once
#include <string>
#include <kapok/Kapok.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class client_proxy : public std::enable_shared_from_this<client_proxy>, private boost::noncopyable
{
public:
	client_proxy(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		: io_service_(io_service),
		socket_(io_service)
	{
		do_connect(endpoint_iterator);
	}

	template<typename... Args>
	void call(const char* handler_name, Args&&... args)
	{
		if(test_str_.empty())
			test_str_ = make_request_json(handler_name, std::forward<Args>(args)...);

		boost::asio::write(socket_, boost::asio::buffer(test_str_.c_str(), test_str_.length()));
	}

private:
	void do_connect(tcp::resolver::iterator endpoint_iterator)
	{
		boost::system::error_code ec;
		boost::asio::connect(socket_, endpoint_iterator, ec);
		if (ec)
			std::cout << ec.message() << std::endl;
		else
			do_read();
	}

	void do_read()
	{
		boost::asio::async_read(socket_, boost::asio::buffer(data_, max_length), [](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				
			}
			else
			{
				//log
				return;
			}
		});
	}

	template<typename T>
	std::string make_request_json(const char* handler_name, T&& t)
	{
		sr_.Serialize(std::forward<T>(t), handler_name);
		return sr_.GetString();
	}

	std::string make_request_json(const char* handler_name)
	{
		return make_request_json(handler_name, "");
	}

	template<typename... Args>
	std::string make_request_json(const char* handler_name, Args&&... args)
	{
		auto tp = std::make_tuple(std::forward<Args>(args)...);
		return make_request_json(handler_name, tp);
	}

private:
	Serializer sr_;
	std::string test_str_ = "";

	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	enum { max_length = 8192 };
	char data_[max_length];
};

