#pragma once

#include <string>
#include <memory>
#include <librdkafka/rdkafka.h>
#include <vector>
#include <capnp/serialize.h>
#include <concepts>
#include <cassert>
#include "SetUpProducerResult.h"

namespace App {
    template <std::invocable Callback>
    class Producer {
    public:
        Producer(const std::string& broker, const std::string& topic, Callback cb) : 
            broker_{std::move(broker)}, topic_{std::move(topic)}, cb_{cb} {};

        ~Producer() {
            rd_kafka_flush(producer_.get(), flash_timeout_ms_);
            rd_kafka_destroy(producer_.get());   
        }
        
        SetUpProducerResult setup_producer() {
            char errstr[512];

            auto conf = rd_kafka_conf_new();
            if (rd_kafka_conf_set(conf, "bootstrap.servers", broker_.c_str(), errstr, sizeof(errstr))
            != RD_KAFKA_CONF_OK) {
                std::cerr << "---> Failed to set broker " << errstr << "\n";
                return SetUpProducerResult::ERROR;
            }

            auto producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
            if (!producer_) {
                std::cerr << "---> Failed to create producer " << errstr << "\n"; 
            }
            
            producer_ = std::shared_ptr<rd_kafka_t>(producer, [](rd_kafka_t* ptr) {
                rd_kafka_destroy(ptr);
            });

            std::cout << "---> Created producer successfully\n";
            return SetUpProducerResult::OK;
        }

        void produce(std::shared_ptr<std::vector<capnp::word>> data) {
            assert(producer_ != nullptr);

            auto buff = static_cast<void*>(data->data());
            rd_kafka_producev(
                producer_.get(),
                RD_KAFKA_V_TOPIC(topic_.c_str()),
                RD_KAFKA_V_VALUE(buff, data->size()),
                RD_KAFKA_V_END
            );

            cb_();
        }

        

    private:
        std::string broker_;
        std::string topic_;
        std::shared_ptr<rd_kafka_t> producer_;
        std::decay_t<Callback> cb_;
        const size_t flash_timeout_ms_ = 10000;
    };
}