#include <iostream>
#include "Client.h"
#include <chrono>
#include <thread>

int main() {
    auto client = Client("9098", "127.0.0.1");

    auto connected = client.connect_to_server();
    if (connect) {
        std::cout << "Connected to server successfully\n"; 

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}