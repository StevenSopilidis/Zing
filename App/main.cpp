#include <iostream>
#include <Server.h>
#include <capnp/serialize.h>
#include <request.capnp.h>
#include <kj/io.h>

    
int main() {
    auto server = App::Server(9098);
    server.run(30);
}