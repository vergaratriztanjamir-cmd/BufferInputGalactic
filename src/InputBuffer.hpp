#pragma once

#include <cstdint>
#include <cstddef>
#include <deque>

namespace Galactic {

struct BufferedInput {
    uint64_t timestampMs;
    int player;
    int mode;
    int button;
    int ticks;
    int frameWindow;
    int bufferWindowMs;
};

class InputBuffer {
private:
    std::deque<BufferedInput> inputs;
    static constexpr size_t MAX_INPUTS = 128;

public:
    void push(
        uint64_t timestampMs,
        int player,
        int mode,
        int button,
        int ticks,
        int frameWindow,
        int bufferWindowMs
    ) {
        if (player < 1 || player > 2) return;

        if (inputs.size() >= MAX_INPUTS) {
            inputs.pop_front();
        }

        inputs.push_back({
            timestampMs,
            player,
            mode,
            button,
            ticks,
            frameWindow,
            bufferWindowMs
        });
    }

    size_t size() const {
        return inputs.size();
    }

    void clear() {
        inputs.clear();
    }
};

}