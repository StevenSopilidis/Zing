#include "Client.h"

int main() {
    ZingClient::Client::simulate_load(1, 100, 100, "127.0.0.1", "9098");
}