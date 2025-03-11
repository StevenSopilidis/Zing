#include <iostream>
#include "KafkaConsumer.h"
#include "SetupConsumerResult.h"

int main() {
    auto consumer = Worker::KafkaConsumer("localhost:9092", "zing_topic", "id");
    auto result = consumer.setup_consumer();
    if (result == SetUpConsumerResult::OK) {
        consumer.run();
    }
}   