
#include <chrono>

#pragma once

class Timer {

public:

    Timer() {
        start();
    }

    void start() {
        restart();
    }

    void restart() {
        lastStart = std::chrono::high_resolution_clock::now();
    }

    unsigned elapsedNanos() {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastStart).count();
        return elapsed;
    }

    unsigned elapseMicros() {
        auto elapsed = elapsedNanos();
        return elapsed / 1000;
    }

    unsigned elapseMilis() {
        auto elapsed = elapsedNanos();
        return elapsed / (1000 * 1000);
    }
    
private:
    std::chrono::high_resolution_clock::time_point lastStart;
};