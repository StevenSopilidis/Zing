#include <iostream>
#include <Server.h>

int main() {
    auto server = Server(8080);
    server.run();
}