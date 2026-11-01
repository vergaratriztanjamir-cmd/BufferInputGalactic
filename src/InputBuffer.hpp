
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
};

class InputBuffer {
private:
    std::deque<BufferedInput> inputs;
    static constexpr size_t MAX_INPUTS = 128;

public:
    void push(uint64_t timestampMs, int player, int mode, int button) {
        if (player < 1 || player > 2) return;

        if (inputs.size() >= MAX_INPUTS) {
            inputs.pop_front();
        }

        inputs.push_back({timestampMs, player, mode, button});
    }

    size_t size() const {
        return inputs.size();
    }

    void clear() {
        inputs.clear();
    }
};

}
