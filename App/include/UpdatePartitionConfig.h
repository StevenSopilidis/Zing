#pragma once

#include <string>

namespace App {
    struct UpdatePartitionConfig{
        std::string topic_name_;
        int new_num_partitions_;
    };
}