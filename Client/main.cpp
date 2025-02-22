#include "Client.h"

int main() {
    ZingClient::Client::simulate_load(100, 1000, 100, "127.0.0.1", "9098");
}