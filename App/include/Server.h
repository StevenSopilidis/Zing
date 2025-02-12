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

    auto run() -> void;

private:
    auto listener(tcp::acceptor acceptor) -> awaitable<void>;
    auto handle_connection(tcp::socket socket) -> awaitable<void>;
    auto process_request_data(const std::vector<uint8_t>& data) -> void;

    const unsigned short port_;
};