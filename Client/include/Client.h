#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <optional>
#include <memory>
#include "request.capnp.h"

using boost::asio::ip::tcp;

struct RequestData {
    uint64_t id;
    uint32_t sourceId;
    uint32_t type;
    const char* data;
};

class Client {
public:
    Client(const std::string& server_port, const std::string& host) : 
        server_port_{std::move(server_port)}, host_{std::move(host)}, io_context_{} {}

    auto connect_to_server() -> bool;
    auto disconnect_from_server() -> void;

    auto send_request(const RequestData& data,const size_t raw_data_size) noexcept -> void;
private:
    auto serialize_request(const RequestData& data,const size_t raw_data_size)
        const noexcept -> std::unique_ptr<std::vector<uint8_t>>;

    const std::string server_port_;
    const std::string host_;
    boost::asio::io_context io_context_;
    std::optional<tcp::socket> server_socket_;
};