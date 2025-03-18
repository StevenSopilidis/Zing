#include <iostream>
#include <Server.h>
#include <capnp/serialize.h>
#include <request.capnp.h>
#include <kj/io.h>  
#include <librdkafka/rdkafka.h>
#include "Kafka.h"
#include "Producer.h"
#include "SetupServerProducerResult.h"

int main() {
    auto server = App::Server(9098, "localhost:9092", "steven_topic2");
    auto result = server.setup_server_producer();
    if (result == SetupServerProducerResult::OK)
        server.run(3);
}