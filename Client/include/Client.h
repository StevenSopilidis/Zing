#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <optional>

using boost::asio::ip::tcp;

class Client {
public:
    Client(const std::string& server_port, const std::string& host) : 
        server_port_{std::move(server_port)}, host_{std::move(host)}, io_context_{} {}

    auto connect_to_server() -> bool;
    auto disconnect_from_server() -> void;
private:
    const std::string server_port_;
    const std::string host_;
    boost::asio::io_context io_context_;
    std::optional<tcp::socket> server_socket_;
};