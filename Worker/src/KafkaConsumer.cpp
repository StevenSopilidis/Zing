#include <iostream>
#include <cassert>
#include "KafkaConsumer.h"

namespace Worker {
    SetUpConsumerResult KafkaConsumer::setup_consumer() noexcept {
        rd_kafka_t* consumer;
        rd_kafka_conf_t* conf = rd_kafka_conf_new();
        char errstr[512];

        if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_.c_str(), errstr, sizeof(errstr))
            != RD_KAFKA_CONF_OK) 
        {
            std::cerr << "---> Failed to set bootstrap.servers: " << errstr << std::endl;
            rd_kafka_conf_destroy(conf);
            return SetUpConsumerResult::ERROR;
        }

        if (rd_kafka_conf_set(conf, "group.id", group_id_.c_str(), errstr, sizeof(errstr))
            != RD_KAFKA_CONF_OK) 
        {
            std::cerr << "---> Failed to set group.id: " << errstr << std::endl;
            rd_kafka_conf_destroy(conf);
            return SetUpConsumerResult::ERROR;
        }

        if (rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr)) 
            != RD_KAFKA_CONF_OK) 
        {
            std::cerr << "---> Failed to set auto.offset.reset: " << errstr << std::endl;
            rd_kafka_conf_destroy(conf);
            return SetUpConsumerResult::ERROR;
        }

        consumer = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
        if (!consumer) {
            std::cerr << "---> Failed to crete Kafka consumer: " << errstr << "\n";
            rd_kafka_conf_destroy(conf);
            return SetUpConsumerResult::ERROR;
        }


        rd_kafka_topic_partition_list_t* topics = rd_kafka_topic_partition_list_new(1);
        if (!topics) {
            std::cerr << "---> Failed to create topic partition list\n";
            rd_kafka_destroy(consumer);
            return SetUpConsumerResult::ERROR;
        }
        rd_kafka_topic_partition_list_add(topics, topic_.c_str(), RD_KAFKA_PARTITION_UA);

        rd_kafka_resp_err_t err = rd_kafka_subscribe(consumer, topics);
        if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
            std::cerr << "---> Failed to subscribe to topic " 
                << topic_ << ": " << rd_kafka_err2str(err) << std::endl;

            rd_kafka_topic_partition_list_destroy(topics);
            rd_kafka_destroy(consumer);
            return SetUpConsumerResult::ERROR;
        }

        rd_kafka_topic_partition_list_destroy(topics);

        consumer_.reset(consumer);
        return SetUpConsumerResult::OK;
    }

    void KafkaConsumer::run() noexcept {
        assert(consumer_);
        
        std::cout << "---> Consuming\n";
        while (true) {
            auto msg = rd_kafka_consumer_poll(consumer_.get(), consumer_poll_timeout_ms_);
            if (msg) {
                if (!msg->err) {
                    std::cout << "---> Received message of size: " << msg->len << "\n";
                }
                rd_kafka_message_destroy(msg);
            }
        }
    }
}