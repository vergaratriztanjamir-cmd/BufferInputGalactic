
#pragma once

#include <cstdint>
#include "InputBuffer.hpp"

namespace Galactic {

struct ProcessedInput {
    BufferedInput input;
    uint64_t processedAtMs;
    uint64_t ageMs;
    bool withinWindow;
};

class InputProcessor {
private:
    uint64_t processedCount = 0;

public:
    bool processNext(
        InputBuffer& buffer,
        uint64_t nowMs,
        ProcessedInput& result
    ) {
        BufferedInput input;

        if (!buffer.consume(input)) {
            return false;
        }

        result.input = input;
        result.processedAtMs = nowMs;

        result.ageMs = nowMs >= input.timestampMs
            ? nowMs - input.timestampMs
            : 0;

        result.withinWindow =
            result.ageMs <=
            static_cast<uint64_t>(input.bufferWindowMs);

        processedCount++;

        return true;
    }

    uint64_t getProcessedCount() const {
        return processedCount;
    }

    void reset() {
        processedCount = 0;
    }
};

}
