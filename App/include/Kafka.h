#pragma once

#include <iostream>
#include <librdkafka/rdkafka.h>
#include <string>
#include "CreateTopicResult.h"
#include "UpdatePartitionsResult.h"
#include "UpdatePartitionConfig.h"
#include "TopicConfig.h"

namespace App {
    class Kafka {
    public:
        Kafka(const std::string& broker) : broker_{std::move(broker)} {};
        Kafka(const Kafka&) = delete;
        Kafka& operator=(Kafka&) = delete;
        Kafka(Kafka&&) = delete;
        Kafka& operator=(Kafka&&) = delete;

        CreateTopicResult create_topic(const TopicConfig& config) noexcept;
        UpdatePartitionResult update_num_partitions(const UpdatePartitionConfig& config) noexcept;

    private:
        std::string broker_;
    };
}