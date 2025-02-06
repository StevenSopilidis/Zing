#include <iostream>
#include <Server.h>
#include <capnp/serialize.h>
#include <request.capnp.h>
#include <kj/io.h>

int main() {
    auto server = Server(8080);
    server.run();
}