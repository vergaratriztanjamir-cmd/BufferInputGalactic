
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

    void update(uint64_t nowMs) {
        while (!inputs.empty()) {
            const auto& input = inputs.front();

            if (nowMs < input.timestampMs ||
                nowMs - input.timestampMs <=
                static_cast<uint64_t>(input.bufferWindowMs)) {
                break;
            }

            inputs.pop_front();
        }
    }

    const BufferedInput* front() const {
        if (inputs.empty()) return nullptr;
        return &inputs.front();
    }

    bool consume(BufferedInput& output) {
        if (inputs.empty()) return false;

        output = inputs.front();
        inputs.pop_front();
        return true;
    }

    size_t size() const {
        return inputs.size();
    }

    bool empty() const {
        return inputs.empty();
    }

    void clear() {
        inputs.clear();
    }
};

}
