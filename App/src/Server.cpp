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
        std::vector<uint8_t> buffer(1024);

        for (;;) {
            auto n = co_await socket.async_read_some(
                boost::asio::buffer(buffer), use_awaitable
            );

            if (n == 0)
                break;
            
            std::cout << "---> " << n << std::endl;

            // Deserialize the request
            process_request_data(buffer);

            co_await boost::asio::async_write(
                socket, 
                boost::asio::buffer("OK", sizeof("OK")),
                use_awaitable
            );
        }
    } catch(const boost::system::system_error& e)
    {
        if (e.code() == boost::asio::error::eof) {
            std::cerr << "---> Connection closed by client" << std::endl;
        } else if (e.code() == boost::asio::error::connection_reset) {
            std::cerr << "---> Connection reset by peer" << std::endl;
        } else {
            std::cerr << "---> Connection error: " << e.what() << std::endl;
        }
    } catch (const std::exception& e)
    {
        std::cerr << "---> Unexpected error: " << e.what() << std::endl;
    }

    try
    {
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }
    catch(const std::exception& e) {}
    
    socket.close();
}

auto Server::process_request_data(const std::vector<uint8_t>& data) -> void {
    auto n = data.size();
    if (data.size() % sizeof(capnp::word) != 0) { // check if data is aligned
        std::cerr << "---> INvalid serialized data size.\n";
        return;
    }

    auto word_count = n / sizeof(capnp::word);
    kj::ArrayPtr<const capnp::word> words(
        reinterpret_cast<const capnp::word*>(data.data()),
        word_count
    );

    capnp::FlatArrayMessageReader message_reader(words);
    Request::Reader request = message_reader.getRoot<Request>();

    std::cout << "---> Received request from: " << request.getSourceId() <<
        " with Id: " << request.getId() << " of Type: " << request.getType() << std::endl; 
}