#include <iostream>
#include "Client.h"
#include <chrono>
#include <thread>
#include <memory>

int main() {
    std::vector<std::thread> threads;
    auto num_of_clients = 10000;

    for (int i = 0; i < num_of_clients; i++)
    {
        auto client = std::make_unique<Client>("9098", "127.0.0.1");
        auto connected = client->connect_to_server();

        if (connected) {
            threads.emplace_back([client = std::move(client)]() {
                const char* test_data = new char[10];

                client->send_request(RequestData{ 
                    id: 10,
                    sourceId: 10,
                    type: 1,
                    data: test_data
                }, 10);

                client->receive_response();
            });
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}