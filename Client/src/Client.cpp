#include "Client.h"
#include <iostream>
#include <capnp/serialize.h>
#include <capnp/message.h>

auto Client::connect_to_server() -> bool {
    try {
        server_socket_.emplace(io_context_);

        tcp::resolver resolver(io_context_);
        tcp::resolver::results_type endpoints = resolver.resolve(host_, server_port_);
        tcp::socket socket(io_context_);
        boost::asio::connect(*server_socket_, endpoints);

        std::cout << "---> Connected to server successfully\n";
    } catch(std::exception& e) {
        std::cout << "---> Could not connecto to server, " << e.what() << "\n";
        return false; 
    }

    return true;
}

auto Client::disconnect_from_server() -> void {
    if (server_socket_ && server_socket_->is_open()) {
        boost::system::error_code ec;
        server_socket_->shutdown(tcp::socket::shutdown_both, ec);

        if (ec)
            std::cerr << "Shutdown error: " << ec.message() << "\n";

        server_socket_->close(ec);
        if (ec)
            std::cerr << "Close error: " << ec.message() << "\n";
        else
            std::cout << "Disconected from server\n";
    }
}

auto Client::serialize_request(const RequestData& data,const size_t raw_data_size) 
    const noexcept -> std::unique_ptr<std::vector<uint8_t>> {
    ::capnp::MallocMessageBuilder message;
    Request::Builder request = message.initRoot<Request>();

    request.setId(data.id);
    request.setSourceId(data.sourceId);
    request.setType(data.type);
    request.setRawBytes(kj::ArrayPtr<const kj::byte>((const kj::byte*)data.data, raw_data_size));

    kj::Array<capnp::word> words = capnp::messageToFlatArray(message);
    auto byteArray = kj::arrayPtr(reinterpret_cast<const uint8_t*>(words.begin()),
                                words.size() * sizeof(capnp::word));

    std::vector<uint8_t> serializedData(byteArray.begin(), byteArray.end());
    return std::make_unique<std::vector<uint8_t>>(serializedData);
}

auto Client::send_request(const RequestData& data,const size_t raw_data_size) noexcept -> void {
    if (server_socket_ && server_socket_->is_open()) {
        try
        {
            auto serialized_data = std::move(serialize_request(data, raw_data_size));

            boost::asio::write(
                *server_socket_, boost::asio::buffer(serialized_data->data(), serialized_data->size())
            );

            printf("---> Request from Source: {%d} was sent successfully\n", data.sourceId);
        }
        catch(const std::exception& e)
        {
            std::cerr << "---> Error sending data: " << e.what() << "\n";
        }
    } else {
        std::cerr << "Socket is not connected or not valid." << "\n";
    }
}