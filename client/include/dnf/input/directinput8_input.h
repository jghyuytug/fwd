#pragma once

#include <cstdint>
#include "dnf/platform/win32.h"
#include "dnf/core/traits.h"

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>

namespace dnf {
namespace input {

// Input creation parameters
struct InputCreateInfo {
    bool mouse_exclusive = false;
    bool foreground = true;
};

// DirectInput8 input handler with keyboard and mouse support
class DirectInput8Input final : private dnf::core::NonCopyable, private dnf::core::NonMovable {
public:
    DirectInput8Input() = default;
    ~DirectInput8Input();

    // Initialize DirectInput8 devices
    bool Initialize(HINSTANCE instance, HWND hwnd, const InputCreateInfo& ci);
    void Shutdown();

    // Update input state (call once per frame)
    void Update();

    // Keyboard state queries (DIK_* constants from dinput.h)
    bool KeyDown(std::uint8_t dik) const noexcept;
    bool KeyPressed(std::uint8_t dik) const noexcept;   // Edge: just pressed this frame
    bool KeyReleased(std::uint8_t dik) const noexcept;  // Edge: just released this frame

    // Mouse state queries
    LONG MouseDeltaX() const noexcept { return mouse_.lX; }
    LONG MouseDeltaY() const noexcept { return mouse_.lY; }
    LONG MouseWheelDelta() const noexcept { return mouse_.lZ; }
    bool MouseButtonDown(int button) const noexcept;  // 0=left, 1=right, 2=middle

private:
    void PollKeyboard();
    void PollMouse();

    IDirectInput8* di_ = nullptr;
    IDirectInputDevice8* keyboard_device_ = nullptr;
    IDirectInputDevice8* mouse_device_ = nullptr;

    std::uint8_t keys_[256] = {};
    std::uint8_t prev_keys_[256] = {};
    DIMOUSESTATE2 mouse_ = {};
    DIMOUSESTATE2 prev_mouse_ = {};
};

}  // namespace input
}  // namespace dnf
