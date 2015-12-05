#ifndef STACK_OVERFLOW_THREAD_H
#define STACK_OVERFLOW_THREAD_H

#include <boost/asio.hpp>
#include <functional>
#include <thread>

class stack_overflow_thread {
public:

	stack_overflow_thread();

	/*
	  Send a message on IRC.
	  This method is thread safe.
	*/
	void send_message(const std::string& message);

	void start(std::function<void(const std::string&)> send_to_irc);

	void join();

private:
	void process_next_messages(std::function<void(const std::string&)> send_to_so);

	int counter = 0;
	boost::asio::io_service service;
	std::thread inner_thread;
};

#endif /* STACK_OVERFLOW_THREAD_H */