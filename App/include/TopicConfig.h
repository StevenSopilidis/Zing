#pragma once

#include <string>

namespace App {
    struct TopicConfig {
        std::string topic_name_;
        int num_partitions_;
        int replication_factor_;
    };
}