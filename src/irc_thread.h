#ifndef IRC_THREAD_H
#define IRC_THREAD_H

#include <boost/asio.hpp>
#include <functional>
#include <thread>

class irc_thread {
public:

	irc_thread();

	/*
	  Send a message on IRC.
	  This method is thread safe.
	*/
	void send_message(const std::string& message);

	void start(std::function<void(const std::string&)> send_to_so);

	void join();

private:
	void handler(std::function<void(const std::string&)> send_to_so, const boost::system::error_code& e, std::size_t size);

	boost::asio::ip::tcp::endpoint get_freenode_endpoint();

	boost::asio::io_service service;
	boost::asio::ip::tcp::socket socket;

	boost::asio::streambuf buffer;

	std::thread my_thread;
};

#endif /* IRC_THREAD_H */