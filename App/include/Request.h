#pragma once

#include <cstdint>
#include <chrono>
#include <array>
#include <atomic>

enum RequestProcessingStage : uint8_t {
    PENDING = 0,
    PROCESSING = 1,
    COMPLETED = 2,
    ERROR = 3,
};

struct alignas(64) Request {
    // Header
    std::uint64_t id;
    std::uint32_t source_id;
    std::uint32_t type;
    std::chrono::nanoseconds created_at;

    // Payload
    std::array<std::byte, 64> raw_bytes;

    // Status/Control fields
    std::atomic<RequestProcessingStage> status;
    uint64_t processed_at;
};