#include "Server.h"
#include "request.capnp.h"
#include <capnp/serialize.h>
#include <kj/array.h>

auto Server::run() -> void {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, {tcp::v4(), port_}, true);
    co_spawn(io_context, listener(std::move(acceptor)), detached);

    std::cout << "---> Server listening at port: " << port_ << "\n";
    io_context.run(); 
}

auto Server::listener(tcp::acceptor acceptor) -> awaitable<void> {
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);

        co_spawn(
            acceptor.get_executor(),
            handle_connection(std::move(socket)),
            detached
        );
    }
}

auto Server::handle_connection(tcp::socket socket) -> awaitable<void> {
    try
    {
        char buffer[1024];

        for (;;) {
            auto n = co_await socket.async_read_some(
                boost::asio::buffer(buffer), use_awaitable
            );

            if (n == 0)
                break;

            kj::ArrayPtr<const capnp::word> words(
                reinterpret_cast<const capnp::word*>(buffer),
                n / sizeof(capnp::word)
            );

            capnp::FlatArrayMessageReader message(words);

            Request::Reader request = message.getRoot<Request>();

            std::cout << request.getId() << "\n";

            co_await boost::asio::async_write(
                socket, 
                boost::asio::buffer("OK", sizeof("OK")),
                use_awaitable
            );
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Connection error: " << e.what() << '\n';
    }


    // handle connection shutdown
    try
    {
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }
    catch(const std::exception& e) {}
    
    socket.close();
}