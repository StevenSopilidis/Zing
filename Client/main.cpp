#include <iostream>
#include "Client.h"
#include <chrono>
#include <thread>
#include <memory>

int main() {
    std::vector<std::thread> threads;
    auto num_of_clients = 100;

    for (int i = 0; i < num_of_clients; i++)
    {
        auto client = std::make_unique<Client>("9098", "127.0.0.1");
        auto connected = client->connect_to_server();
        if (connect) {
            printf("---> Client %d connected to server successfully\n", i);
        }

        threads.emplace_back([client = std::move(client)]() {
            const char* test_data = new char[10];

            client->send_request(RequestData{ 
                id: 10,
                sourceId: 10,
                type: 1,
                data: test_data
            }, 10);
        });
    }

    for (auto& t : threads) {
        t.join();
    }
}