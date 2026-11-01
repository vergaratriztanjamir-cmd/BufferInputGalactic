
#pragma once

#include <cstdint>
#include <deque>
#include <algorithm>

namespace Galactic {

struct BufferedInput {
    uint64_t frame;
    int player;
    int mode;
};

class InputBuffer {
private:
    std::deque<BufferedInput> inputs;
    static constexpr size_t MAX_INPUTS = 128;

public:
    void push(
        uint64_t frame,
        int player,
        int mode
    ) {
        if (player < 1 || player > 2)
            return;

        if (inputs.size() >= MAX_INPUTS)
            inputs.pop_front();

        inputs.push_back({
            frame,
            player,
            mode
        });
    }

    bool consume(
        uint64_t frame,
        int player,
        int mode,
        int window
    ) {
        auto it = std::find_if(
            inputs.begin(),
            inputs.end(),
            [&](const BufferedInput& input) {
                return input.player == player &&
                    input.mode == mode &&
                    frame >= input.frame &&
                    frame - input.frame <=
                        static_cast<uint64_t>(
                            std::max(0, window)
                        );
            }
        );

        if (it == inputs.end())
            return false;

        inputs.erase(it);
        return true;
    }

    void clear() {
        inputs.clear();
    }

    size_t size() const {
        return inputs.size();
    }
};

}
