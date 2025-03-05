#pragma once

namespace App {
    enum class CreateTopicResult {
        OK,
        ERROR,
        TOPIC_ALREADY_EXISTS
    };
}