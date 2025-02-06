#include "Client.h"
#include <iostream>


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
