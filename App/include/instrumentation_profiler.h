#pragma once
#include <chrono>
#include <iostream>

namespace App {
    #define USER_TIMER 1

    #ifdef USER_TIMER
    #define MEASURE_FUNCTION() ScopedTimer timer{ __func__ }
    #else
    #define MEASURE_FUNCTION()
    #endif


    class ScopedTimer {
    public:
        using ClockType = std::chrono::steady_clock;

        ScopedTimer(const char* func): 
            function_name_(func), start_time_(ClockType::now()) {}

        ScopedTimer(const ScopedTimer& timer) = delete;
        ScopedTimer(ScopedTimer&& timer) = delete;
        auto operator=(const ScopedTimer& timer) -> ScopedTimer& = delete;
        auto operator=(ScopedTimer&& timer) -> ScopedTimer& = delete;

        void stop() {
            using namespace std::chrono;
            auto stop_time = ClockType::now();
            duration_ = 
                std::chrono::duration_cast<std::chrono::microseconds>
                (stop_time - start_time_).count();
        }

        auto get_elapsed_time() -> long long {
            if (!stopped_)
                stop();
            return duration_;
        }

        ~ScopedTimer() {
            stop();
            std::cout 
                << "---> " << function_name_
                << " executed in: " 
                << duration_
                << "microseconds\n";
        }

    private:
        const char* function_name_{};
        const ClockType::time_point start_time_{};
        long long duration_{};
        bool stopped_{false};
    };
}