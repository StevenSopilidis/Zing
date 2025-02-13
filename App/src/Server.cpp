#include "Server.h"
#include "request.capnp.h"
#include <capnp/serialize.h>
#include <kj/array.h>

auto Server::run() -> void {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint{udp::v4(), port_});
    co_spawn(io_context, listener(std::move(socket)), detached);

    std::cout << "---> Server listening at port: " << port_ << "\n";
    io_context.run(); 
}

auto Server::listener(udp::socket socket) -> awaitable<void> {
    for (;;) {
        std::vector<uint8_t> buffer(1024);
        udp::endpoint remote_endpoint;
        
        std::size_t n = co_await socket.async_receive_from(
            boost::asio::buffer(buffer), remote_endpoint, use_awaitable
        );

        buffer.resize(n);
        std::cout << "---> Received " << n << " bytes from " 
                  << remote_endpoint.address().to_string() << "\n";

        process_request_data(buffer);

        co_await socket.async_send_to(
            boost::asio::buffer("OK", 3),
            remote_endpoint,
            use_awaitable
        );
    }
}

auto Server::process_request_data(const std::vector<uint8_t>& data) -> void {
    auto n = data.size();
    if (data.size() % sizeof(capnp::word) != 0) { // check if data is aligned
        std::cerr << "---> Invalid serialized data size.\n";
        return;
    }

    auto word_count = n / sizeof(capnp::word);
    kj::ArrayPtr<const capnp::word> words(
        reinterpret_cast<const capnp::word*>(data.data()),
        word_count
    );

    capnp::FlatArrayMessageReader message_reader(words);
    Request::Reader request = message_reader.getRoot<Request>();

    std::cout << "---> Received request from: " << request.getSourceId() <<
        " with Id: " << request.getId() << " of Type: " << request.getType() << std::endl; 
}