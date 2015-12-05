#include "irc_thread.h"
#include <iostream>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

irc_thread::irc_thread() : socket(service) {
}

const char* room_name = "#test1";

void irc_thread::send_message(const std::string& message) {
	service.dispatch([this, message](){
		std::string temp = (boost::format("PRIVMSG %1% :%2% \r\n") % room_name % message).str();
		boost::asio::write(socket, boost::asio::buffer(temp));
	});
}

void irc_thread::handler(std::function<void(const std::string&)> send_to_so, const boost::system::error_code& error, std::size_t size)
{
	if (error)
  	{
  		std::cerr<<"I got an error " << error << std::endl;
  		exit(1);
  	}


    std::istream is(&buffer);
    std::string line;
    std::getline(is, line);

    std::string username = "";

    std::cout<<"Original line"<<line<<std::endl;

   	if (line[0] == ':') {
   		int last_index_of_username = line.find_first_of("! ");
   		username = line.substr(1, (last_index_of_username - 1));
   		line = line.substr(line.find(' ')+1);
   	}

   	std::cout<<"Username is " << username << std::endl;
   	std::cout<<"Read something "<< line <<std::endl;

   	std::string command = line.substr(0, line.find(' '));
   	line = line.substr(line.find(' ')+1);

   	if (command == "PRIVMSG") {
   		std::string channel = line.substr(0, line.find(' '));
   		line = line.substr(line.find(' ')+1);

   		if (line[0] == ':') {
   			line = line.substr(1);
   		}
   		std::cout<<"sending " << line <<std::endl;
   		send_to_so((boost::format("%1%: %2%") % username % line).str());
   	} else if (command == "PING") {
   		std::cout<<"Responded to ping"<<std::endl;
   		boost::asio::write(socket, boost::asio::buffer(std::string("PONG loungebot.example.net")));
   	}


    boost::asio::async_read_until(socket, buffer, "\r\n", std::bind(&irc_thread::handler, this, send_to_so, std::placeholders::_1, std::placeholders::_2));
}


const std::string url = "chat.freenode.net";
const int port = 6665;

boost::asio::ip::tcp::endpoint irc_thread::get_freenode_endpoint() {
	boost::asio::ip::tcp::resolver resolver(service);
	boost::asio::ip::tcp::resolver::query query(url, "");
	for(boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
	                            i != boost::asio::ip::tcp::resolver::iterator();
	                            ++i)
	{
		auto result = i->endpoint();
		result.port(port);
	    return result;
	}
	
	std::cerr<<"No such endpoint for that url" <<std::endl;

	exit(1);
}

void irc_thread::start(std::function<void(const std::string&)> send_to_so) {

	boost::asio::ip::tcp::endpoint endpoint = get_freenode_endpoint();
	socket.connect(endpoint);

  	boost::asio::write(socket, boost::asio::buffer(std::string("NICK loungebot12 \r\n")));
	boost::asio::write(socket, boost::asio::buffer(std::string("USER loungebot12 0 * :The lounge bot \r\n")));
	boost::asio::write(socket, boost::asio::buffer(std::string("JOIN #test1 \r\n")));
	
	boost::asio::async_read_until(socket, buffer, "\r\n", std::bind(&irc_thread::handler, this, send_to_so, std::placeholders::_1, std::placeholders::_2));

	std::cout<<"Should have started?"<< endpoint<<std::endl;

	my_thread = std::thread([this](){
		service.run();
	});
}

void irc_thread::join() {
	my_thread.join();
}