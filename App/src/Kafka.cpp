#include "Kafka.h"

namespace App {
    CreateTopicResult Kafka::create_topic(const TopicConfig& config) noexcept {
        char errstr[512];
        
        auto conf = rd_kafka_conf_new();
        if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_.c_str(), errstr, sizeof(errstr))
            != RD_KAFKA_CONF_OK) {
            std::cerr << "---> Failed to set broker: " << errstr << "\n";
            return CreateTopicResult::ERROR;
        }        


        auto producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
        if (!producer) {
            std::cerr << "---> Failed to create producer: " << errstr << "\n";
            return CreateTopicResult::ERROR;
        }

        auto new_topic = rd_kafka_NewTopic_new(
            config.topic_name_.c_str(), config.num_partitions_, 
            config.replication_factor_, errstr, sizeof(errstr)
        );
        if (!new_topic) {
            rd_kafka_destroy(producer);
            std::cerr << "---> Failed to create new topic: " << errstr << "\n";
            return CreateTopicResult::ERROR;
        }

        auto options = rd_kafka_AdminOptions_new(producer, RD_KAFKA_ADMIN_OP_ANY);
        auto queue = rd_kafka_queue_new(producer);

        rd_kafka_NewTopic_t *topics[] = { new_topic };
        rd_kafka_CreateTopics(producer, topics, 1, options, queue);

        std::cout << "---> Request topic creation: " << config.topic_name_ 
            << " with " << config.num_partitions_ << " partitions\n";
        
        rd_kafka_event_t *event = rd_kafka_queue_poll(queue, 10000);

        auto created = CreateTopicResult::ERROR;

        if (event && rd_kafka_event_type(event) == RD_KAFKA_EVENT_CREATETOPICS_RESULT) {
            const auto *result = rd_kafka_event_CreateTopics_result(event);
            const rd_kafka_topic_result_t **topic_results;
            size_t topic_result_count;

            topic_results = rd_kafka_CreateTopics_result_topics(result, &topic_result_count);
            
            for (size_t i = 0; i < topic_result_count; i++) {
                const auto topic_result = topic_results[i];
                auto error = rd_kafka_topic_result_error(topic_result);

                if (error == RD_KAFKA_RESP_ERR_NO_ERROR) {
                    std::cout << "---> Topic created successfully: " 
                        << rd_kafka_topic_result_name(topic_result) << std::endl;
                    created = CreateTopicResult::OK;
                } else if (error == RD_KAFKA_RESP_ERR_TOPIC_ALREADY_EXISTS) { 
                    std::cout << "---> Topic already exists" << "\n";
                    created = CreateTopicResult::TOPIC_ALREADY_EXISTS;
                } else {
                    std::cerr << "---> Failed to create topic: " 
                        << rd_kafka_topic_result_name(topic_result)
                        << " Error: " << rd_kafka_topic_result_error_string(topic_result) << std::endl;
                }
            }
        } else {
            std::cerr << "--> Error: No response from Kafka while creating topic." << std::endl;
        }

        if (event)
            rd_kafka_event_destroy(event);
        rd_kafka_queue_destroy(queue);
        rd_kafka_AdminOptions_destroy(options);
        rd_kafka_destroy(producer);

        return created; 
    }


    UpdatePartitionResult Kafka::update_num_partitions(const UpdatePartitionConfig& config) noexcept {
        char errstr[512];
        
        auto conf = rd_kafka_conf_new();
        if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_.c_str(), errstr, sizeof(errstr))
            != RD_KAFKA_CONF_OK) {
            std::cerr << "---> Failed to set broker: " << errstr << "\n";
            return UpdatePartitionResult::ERROR;
        }        


        auto producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
        if (!producer) {
            std::cerr << "---> Failed to create producer: " << errstr << "\n";
            return UpdatePartitionResult::ERROR;
        }

        auto new_partitions = rd_kafka_NewPartitions_new(
            config.topic_name_.c_str(), config.new_num_partitions_, errstr, sizeof(errstr));

        if (!new_partitions) {
            rd_kafka_destroy(producer);
            std::cerr << "---> Failed to create new partitions: " << errstr << "\n";
            return UpdatePartitionResult::ERROR;
        }

        auto options = rd_kafka_AdminOptions_new(producer, RD_KAFKA_ADMIN_OP_ANY);
        auto queue = rd_kafka_queue_new(producer);

        rd_kafka_NewPartitions_t *partitions[] = { new_partitions };
        rd_kafka_CreatePartitions(producer, partitions, 1, options, queue);

        std::cout << "---> Request partition num update for topic : " << config.topic_name_ 
            << " with new partition num: " << config.new_num_partitions_ << " partitions\n";
        
        rd_kafka_event_t *event = rd_kafka_queue_poll(queue, 10000);
        auto updated = UpdatePartitionResult::ERROR;

        if (event && rd_kafka_event_type(event) == RD_KAFKA_EVENT_CREATEPARTITIONS_RESULT) {
            const auto result = rd_kafka_event_CreatePartitions_result(event);
            size_t topic_count;
            const auto topic_results = rd_kafka_CreatePartitions_result_topics(result, &topic_count);
            for (size_t i = 0; i < topic_count; ++i) {
                const rd_kafka_topic_result_t *topic_result = topic_results[i];
                rd_kafka_resp_err_t err = rd_kafka_topic_result_error(topic_result);
                if (err == RD_KAFKA_RESP_ERR_NO_ERROR) {
                    std::cout << "---> Successfully altered partitions for topic: " 
                        << rd_kafka_topic_result_name(topic_result) << std::endl;
                    updated = UpdatePartitionResult::OK;
                } else {
                    std::cerr << "---> Failed to alter partitions for topic: " 
                        << rd_kafka_topic_result_name(topic_result)
                        << " Error: " << rd_kafka_topic_result_error_string(topic_result) << std::endl;
                }
            }
        } else {
            std::cerr << "---> Error: No valid response from Kafka when altering partitions." << std::endl;
        }

        if (event)
            rd_kafka_event_destroy(event);
        rd_kafka_queue_destroy(queue);
        rd_kafka_AdminOptions_destroy(options);
        rd_kafka_destroy(producer);

        return updated; 
    }
}