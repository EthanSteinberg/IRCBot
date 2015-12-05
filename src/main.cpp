#include <iostream>
#include "stack_overflow_thread.h"
#include "irc_thread.h"

int main() {
    std::cout<<"Hello world"<<std::endl;

    stack_overflow_thread so_thread;
    irc_thread ir_thread;

    ir_thread.start([&](auto m){
    	so_thread.send_message(m);
    });

    so_thread.start([&](auto m){
    	ir_thread.send_message(m);
    });

    so_thread.send_message("Bot is online");
    ir_thread.send_message("Bot is online");

    ir_thread.join();
    so_thread.join();

}
