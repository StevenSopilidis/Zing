#include "Server.h"
#include "request.capnp.h"
#include <capnp/serialize.h>
#include <kj/array.h>
#include <array>
#include <thread>

namespace App {
    using boost::asio::awaitable;
    using boost::asio::co_spawn;
    using boost::asio::detached;
    using boost::asio::use_awaitable;
    using boost::asio::ip::udp;

    void Server::run(size_t num_threads) {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint{udp::v4(), port_});
        co_spawn(io_context, listener(std::move(socket)), detached);

        std::cout << "---> Server listening at port: " << port_ << "\n";
        std::vector<std::thread> threads;
        for (size_t i = 0; i < num_threads; i++)
        {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    awaitable<void> Server::listener(udp::socket socket) {
        try
        {
            for (;;) {
                auto buffer = std::make_shared<std::vector<uint8_t>>(1024);
                udp::endpoint remote_endpoint;

                std::size_t n = co_await socket.async_receive_from(
                    boost::asio::buffer(*buffer), remote_endpoint, use_awaitable
                );

                std::cout << "---> Received " << n << " bytes from " 
                        << remote_endpoint.address().to_string() << "\n";

                co_spawn(socket.get_executor(), 
                    [this, socket_ptr=&socket, buffer, remote_endpoint]() -> awaitable<void> {
                        process_request_data(buffer);

                        co_await socket_ptr->async_send_to(
                        boost::asio::buffer("OK", 3),
                        remote_endpoint,
                        use_awaitable
                    );
                }, detached);
            }
        }catch(const std::exception& e)
        {
            std::cerr << "---> " << e.what() << '\n';
        }
    }

    void Server::process_request_data(std::shared_ptr<std::vector<uint8_t>> data) {
        auto n = data->size();
        if (data->size() % sizeof(capnp::word) != 0) { // check if data is aligned
            std::cerr << "---> Invalid serialized data size.\n";
            return;
        }

        auto word_count = n / sizeof(capnp::word);
        kj::ArrayPtr<const capnp::word> words(
            reinterpret_cast<const capnp::word*>(data->data()),
            word_count
        );

        capnp::FlatArrayMessageReader message_reader(words);
        Request::Reader request = message_reader.getRoot<Request>();

        std::cout << "---> Received request from: " << request.getSourceId() <<
            " with Id: " << request.getId() << " of Type: " << request.getType() << std::endl; 
    }
}