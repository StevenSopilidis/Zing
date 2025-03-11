#pragma once

#include <string>
#include <memory>
#include <librdkafka/rdkafka.h>
#include "SetupConsumerResult.h"

namespace Worker {
    class KafkaConsumer {
    public:
        explicit KafkaConsumer(
            const std::string& broker, 
            const std::string& topic, 
            const std::string& group_id
        ) : 
        broker_{std::move(broker)}, topic_{std::move(topic)}, group_id_{std::move(group_id)} {}

        ~KafkaConsumer() {
            rd_kafka_consumer_close(consumer_.get());
        }

        SetUpConsumerResult setup_consumer() noexcept;
        void run() noexcept;

    private:
        std::string broker_;
        std::string topic_;
        std::string group_id_;
        const size_t consumer_poll_timeout_ms_ = 50;
        std::unique_ptr<rd_kafka_t, decltype(&rd_kafka_destroy)> consumer_{nullptr, &rd_kafka_destroy};
    };
}