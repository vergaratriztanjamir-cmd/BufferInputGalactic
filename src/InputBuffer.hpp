#pragma once
#include <cstdint>
#include <deque>
#include <mutex>

namespace Galactic {
struct BufferedInput {
    int player = 1;
    int mode = 0;
    bool down = false;
    uint64_t timestampMs = 0;
    int ticks = 1;
    int frameWindow = 2;
    int bufferWindowMs = 50;
};

class InputBuffer {
private:
    std::deque<BufferedInput> queue;
    mutable std::mutex mutex;
    bool buttonDown[2] = {false, false};
    static constexpr std::size_t MaxQueuedInputs = 256;

public:
    bool record(const BufferedInput& input) {
        if (input.player < 1 || input.player > 2) return false;
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.size() >= MaxQueuedInputs) queue.pop_front();
        queue.push_back(input);
        buttonDown[input.player - 1] = input.down;
        return true;
    }

    bool consume(BufferedInput& output) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) return false;
        output = queue.front();
        queue.pop_front();
        return true;
    }

    bool isDown(int player) const {
        if (player < 1 || player > 2) return false;
        std::lock_guard<std::mutex> lock(mutex);
        return buttonDown[player - 1];
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        queue.clear();
        buttonDown[0] = buttonDown[1] = false;
    }
};
}
