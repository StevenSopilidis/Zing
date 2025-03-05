#include <iostream>
#include <Server.h>
#include <capnp/serialize.h>
#include <request.capnp.h>
#include <kj/io.h>  
#include <librdkafka/rdkafka.h>
#include "Kafka.h"

int main() {
    auto config = App::UpdatePartitionConfig{"test_topic", 1};
    auto kafka = App::Kafka("localhost:9092");
    // kafka.create_topic(config);
    kafka.update_num_partitions(config);
}