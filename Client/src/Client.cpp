#include "Client.h"
#include <iostream>
#include <capnp/serialize.h>
#include <capnp/message.h>
#include <memory>
#include <thread>
#include <memory>
#include <instrumentation_profiler.h>

namespace ZingClient {
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


    auto Client::simulate_load(
        size_t num_threads,
        size_t requests_per_thread, 
        size_t data_size,
        const std::string& server_host,
        const std::string& server_port) -> void 
    {
        auto total_time = 0;
        auto num_requests = std::atomic<int>{0};
        auto total_duration = std::atomic<long long>{0}; 

        for (int i = 0; i < num_threads; i++)
        {
            auto client = std::make_unique<Client>(server_host, server_port);
            auto connected = client->connect_to_server();

            if (connected) {
                std::jthread([
                    client = std::move(client),
                    data_size,
                    &num_requests,
                    requests_per_thread,
                    &total_duration
                ]() {
                    for (int i = 0; i < requests_per_thread; i++) {
                        auto timer = App::ScopedTimer("client_request");

                        const char* test_data = new char[data_size];

                        client->send_request(RequestData{ 
                            id: 10,
                            sourceId: 10,
                            type: 1,
                            data: test_data
                        }, 10);

                        client->receive_response();

                        auto elapsed_time = timer.get_elapsed_time();
                        num_requests.fetch_add(1, std::memory_order_relaxed);
                        total_duration.fetch_add(elapsed_time, std::memory_order_relaxed);
                    }
                });
            }
        }

        auto average_request_time = total_duration / num_requests;

        std::cout << "---------------------------------\n";
        std::cout << "---> Metrics\n";
        std::cout << "---> Num of requests: " << num_requests << "\n";
        std::cout << "---> Average request duration: " << average_request_time << "ms\n";
        std::cout << "---------------------------------\n";
    };
}