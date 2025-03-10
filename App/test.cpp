#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <librdkafka/rdkafka.h>

#define BROKER "localhost:9092"
#define TOPIC "zing_topic"
#define GROUP_ID "test-group"

bool run_consumer = true;

// Kafka Consumer Function
void kafka_consumer() {
    rd_kafka_t *consumer;
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    char errstr[512];

    rd_kafka_conf_set(conf, "bootstrap.servers", BROKER, errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "group.id", GROUP_ID, errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr));

    consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    rd_kafka_poll_set_consumer(consumer);

    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, TOPIC, RD_KAFKA_PARTITION_UA);
    rd_kafka_subscribe(consumer, topics);

    while (run_consumer) {
        rd_kafka_message_t *msg = rd_kafka_consumer_poll(consumer, 1000);
        if (msg) {
            if (!msg->err) {
                std::cout << "Consumed: " << std::string((char *)msg->payload, msg->len) << std::endl;
            }
            rd_kafka_message_destroy(msg);
        }
    }

    rd_kafka_consumer_close(consumer);
    rd_kafka_destroy(consumer);
}

// Signal handler to stop consumer gracefully
void signal_handler(int signal) {
    run_consumer = false;
}

int main() {
    std::signal(SIGINT, signal_handler);

    std::thread consumer_thread(kafka_consumer);

    consumer_thread.join();

    std::cout << "Exiting..." << std::endl;
    return 0;
}
