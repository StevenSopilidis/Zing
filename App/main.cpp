#include <iostream>
#include <Server.h>
#include <capnp/serialize.h>
#include <request.capnp.h>
#include <kj/io.h>  
#include <librdkafka/rdkafka.h>
#include "Kafka.h"
#include "Producer.h"

int main() {
    auto server = App::Server(9098, "localhost:9092", "zing_topic");
    server.run(3);
}