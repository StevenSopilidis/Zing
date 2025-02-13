#include "Client.h"
#include <iostream>
#include <capnp/serialize.h>
#include <capnp/message.h>
#include <memory>

auto Client::connect_to_server() -> bool {
    try {
        udp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(server_host_, server_port_);

        server_socket_.emplace(io_context_);
        
        // set default endpoint
        server_socket_->connect(*endpoints.begin());

        std::cout << "---> Connected to server successfully (UDP)\n";
    } catch(std::exception& e) {
        std::cout << "---> Could not connect to server, " << e.what() << "\n";
        return false; 
    }

    return true;
}

auto Client::disconnect_from_server() -> void {
    // just close udp socket
    if (server_socket_ && server_socket_->is_open()) {
        boost::system::error_code ec;
        server_socket_->close(ec);

        if (ec)
            std::cerr << "Close error: " << ec.message() << "\n";
        else
            std::cout << "Disconnected from server\n";
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

            server_socket_->send(boost::asio::buffer(serialized_data->data(), serialized_data->size()));

            printf("---> Request from Source: {%d} was sent successfully\n", data.sourceId);
        }
        catch(const std::exception& e)
        {
            std::cerr << "---> Error sending data: " << e.what() << "\n";
        }
    } else {
        std::cerr << "---> Socket is not connected or not valid." << "\n";
    }
}

auto Client::receive_response() -> std::unique_ptr<std::vector<uint8_t>> {
    if (server_socket_ && server_socket_->is_open()) {
        try
        {
            std::vector<uint8_t> buffer(1024);
            boost::system::error_code ec;

            size_t n = server_socket_->receive(boost::asio::buffer(buffer), 0, ec);
            if (ec && ec != boost::asio::error::message_size) {
                std::cerr << "---> Error receiving response: " << ec.message() << "\n";
            }

            buffer.resize(n);
            std::cout << "---> Received data from server of size: " << n << "\n";
            return std::make_unique<std::vector<uint8_t>>(std::move(buffer)); 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return nullptr;
        }
    } else {
        std::cerr << "---> Socket is not connected or not valid." << "\n";
        return nullptr;
    }
}