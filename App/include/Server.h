#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <iostream>
#include <stddef.h>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

class Server {
public:
    Server(const unsigned short port) : port_{port} {}

    Server(Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&) = delete;
    Server& operator=(Server&&) = delete;

    auto run() -> void {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, {tcp::v4(), port_}, true);
        co_spawn(io_context, listener(std::move(acceptor)), detached);

        std::cout << "---> Server listening at port: " << port_ << "\n";
        io_context.run(); 
    }

private:
    awaitable<void> listener(tcp::acceptor acceptor) {
        for(;;) {
            auto socket = co_await acceptor.async_accept(use_awaitable);
        }
    }

    const unsigned short port_;
};