#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <iostream>
#include <capnp/serialize.h>
#include <stddef.h>
#include <span>
#include <memory>
#include "Producer.h"

namespace App {
    class Server final {
    public:
        Server(const unsigned short port,
            const std::string& broker,
            const std::string& topic) noexcept 
            : port_{port},
            producer_{broker, topic, produce_callback} 
        {
            producer_.setup_producer();
        }

        Server(Server&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&) = delete;
        Server& operator=(Server&&) = delete;

        void run(size_t num_threads);

    private:
        boost::asio::awaitable<void> listener(boost::asio::ip::udp::socket acceptor);
        void process_request_data(
            std::shared_ptr<std::vector<capnp::word>> data, std::size_t bytes_received
        );

        static void produce_callback() {
            std::cout << "---> Message produced\n";
        }

        const unsigned short port_;
        Producer<decltype(produce_callback)> producer_;
    };        
}

