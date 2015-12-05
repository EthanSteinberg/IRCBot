#include "stack_overflow_thread.h"

#include <curl/curl.h>
#include <iostream>
#include <rapidjson/document.h>
#include <boost/format.hpp>

const int bot_id = 5637979;
const int room_id = 97015;

stack_overflow_thread::stack_overflow_thread() {
	curl_global_init(CURL_GLOBAL_SSL);
}

void stack_overflow_thread::send_message(const std::string& message) {
	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cerr<<"Curl was null"<<std::endl;
	}

	auto url = boost::format("http://chat.stackoverflow.com/chats/%1%/messages/new") % room_id;
	auto url_str = url.str();
	curl_easy_setopt(curl, CURLOPT_URL, url_str.c_str());

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "foo3.txt");

	auto post_data = boost::format("text=%1%&fkey=58344df6f6a6c6e3bed5483913ad2019") % message;
	auto post_data_str = post_data.str();

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data_str.c_str());
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
}

size_t callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* data = (std::string*)userdata;

	for (int i = 0; i < size * nmemb; i++) {
		data->push_back(ptr[i]);
	}

	return size * nmemb;
}

void stack_overflow_thread::process_next_messages(std::function<void(const std::string&)> send_to_so) {
	CURL *curl = curl_easy_init();

	if (!curl) {
		std::cerr<<"Curl was null"<<std::endl;
	}

	curl_easy_setopt(curl, CURLOPT_URL, "http://chat.stackoverflow.com/events");
	curl_easy_setopt(curl, CURLOPT_POST, 1);

	char temp[1000];
	snprintf(temp, sizeof(temp),"fkey=f2c3e8fffaf8cbf8f1e139436d293ca4&r97015=%d",counter);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp);

	std::string result;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&result);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	std::cout<<"Got "<<result<<std::endl;

	rapidjson::Document document;
	document.Parse(result.c_str());

	const rapidjson::Value& value = document["r97015"];
	
	if (counter != 0 && value.HasMember("e")) {
		for (auto it = value["e"].Begin(); it != value["e"].End(); it++) {
			auto& event = *it;

			if (event["event_type"].GetInt() == 1 && event["user_id"].GetInt() != 5637979) {
				std::string message = (boost::format("%1%: %2%") % event["user_name"].GetString() % event["content"].GetString()).str();
				send_to_so(message);
			}
			
		}
	}

	if (value.HasMember("t")) {
		counter = value["t"].GetInt();
	}
	
	std::shared_ptr<boost::asio::deadline_timer> timer = std::make_shared<boost::asio::deadline_timer>(service);

	// Set an expiry time relative to now.
	timer->expires_from_now(boost::posix_time::seconds(1));

	// Wait for the timer to expire.
	timer->async_wait([this, timer, send_to_so](const boost::system::error_code& error){
		if (error) {
			std::cerr<<"The error was"<<error<<std::endl;
			exit(1);
		}
		process_next_messages(send_to_so);
	});
}

void stack_overflow_thread::start(std::function<void(const std::string&)> send_to_so) {
	service.dispatch([this, send_to_so](){
		process_next_messages(send_to_so);
	});

	inner_thread = std::thread([this](){
		service.run();
	});
}

void stack_overflow_thread::join() {
	inner_thread.join();
}