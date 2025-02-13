#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <optional>
#include <memory>
#include "request.capnp.h"

using boost::asio::ip::udp;

struct RequestData {
    uint64_t id;
    uint32_t sourceId;
    uint32_t type;
    const char* data;
};

class Client {
public:
    Client(const std::string& server_port, const std::string& server_host) : 
        server_port_{std::move(server_port)}, 
        server_host_{std::move(server_host)},
        io_context_{} {}

    auto connect_to_server() -> bool;
    auto disconnect_from_server() -> void;

    auto send_request(const RequestData& data,const size_t raw_data_size) noexcept -> void;
    auto receive_response() -> std::unique_ptr<std::vector<uint8_t>>;
private:
    auto serialize_request(const RequestData& data,const size_t raw_data_size)
        const noexcept -> std::unique_ptr<std::vector<uint8_t>>;

    const std::string server_port_;
    const std::string server_host_;
    boost::asio::io_context io_context_;
    std::optional<udp::socket> server_socket_;
};