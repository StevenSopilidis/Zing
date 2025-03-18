#pragma once

#include <memory>
#include "request.capnp.h"
#include <capnp/serialize.h>
#include <iostream>

auto parse_payload_to_request(std::shared_ptr<const uint8_t> payload, size_t payload_len) 
-> std::unique_ptr<Request::Reader> {
    size_t numWords = (payload_len + sizeof(capnp::word) - 1) / sizeof(capnp::word);
    auto buffer = kj::heapArray<capnp::word>(numWords);

    memcpy(buffer.asBytes().begin(), payload.get(), payload_len);

    try {
        capnp::FlatArrayMessageReader messageReader(buffer.asPtr());
        Request::Reader request = messageReader.getRoot<Request>();
        
        std::cout << "---> Received RequestID: " << request.getId() << "\n";
        return std::make_unique<Request::Reader>(request);
    } catch (const kj::Exception& e) {
        std::cerr << "Cap'n Proto parsing error: " 
            << e.getDescription().cStr() << std::endl;
        
        return nullptr;
    }

}