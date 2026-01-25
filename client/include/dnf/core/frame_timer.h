#pragma once

#include "dnf/platform/win32.h"

namespace dnf {
namespace core {

// Lightweight frame timer based on QPC.
// Header-only to keep build integration simple at this stage.
class FrameTimer {
public:
    FrameTimer() { Reset(); }

    void Reset() noexcept {
        valid_ = (::QueryPerformanceFrequency(&freq_) != 0);
        (void)::QueryPerformanceCounter(&last_);
        elapsed_seconds_ = 0.0;
    }

    // Returns delta time in seconds (clamped).
    float Tick() noexcept {
        LARGE_INTEGER now{};
        (void)::QueryPerformanceCounter(&now);

        double dt = 0.0;
        if (valid_ && freq_.QuadPart > 0) {
            dt = static_cast<double>(now.QuadPart - last_.QuadPart) /
                 static_cast<double>(freq_.QuadPart);
        }

        last_ = now;

        if (dt < 0.0) {
            dt = 0.0;
        }
        if (dt > 0.25) {
            dt = 0.25;  // Clamp for stability after debugger breaks / long stalls.
        }

        elapsed_seconds_ += dt;
        return static_cast<float>(dt);
    }

    double elapsed_seconds() const noexcept { return elapsed_seconds_; }

private:
    LARGE_INTEGER freq_ = {};
    LARGE_INTEGER last_ = {};
    bool valid_ = false;
    double elapsed_seconds_ = 0.0;
};

}  // namespace core
}  // namespace dnf
