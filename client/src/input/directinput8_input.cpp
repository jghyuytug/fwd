#include "dnf/input/directinput8_input.h"
#include <cstring>

namespace dnf {
namespace input {

DirectInput8Input::~DirectInput8Input() {
    Shutdown();
}

bool DirectInput8Input::Initialize(HINSTANCE instance, HWND hwnd, const InputCreateInfo& ci) {
    Shutdown();

    // Create DirectInput8 interface
    HRESULT hr = ::DirectInput8Create(
        instance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void**>(&di_),
        nullptr);

    if (FAILED(hr)) {
        return false;
    }

    // Create keyboard device
    hr = di_->CreateDevice(GUID_SysKeyboard, &keyboard_device_, nullptr);
    if (FAILED(hr)) {
        return false;
    }

    hr = keyboard_device_->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr)) {
        return false;
    }

    DWORD kb_flags = 0;
    kb_flags |= ci.foreground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
    kb_flags |= DISCL_NONEXCLUSIVE;

    hr = keyboard_device_->SetCooperativeLevel(hwnd, kb_flags);
    if (FAILED(hr)) {
        return false;
    }

    // Create mouse device
    hr = di_->CreateDevice(GUID_SysMouse, &mouse_device_, nullptr);
    if (FAILED(hr)) {
        return false;
    }

    hr = mouse_device_->SetDataFormat(&c_dfDIMouse2);
    if (FAILED(hr)) {
        return false;
    }

    DWORD mouse_flags = 0;
    mouse_flags |= ci.foreground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
    mouse_flags |= ci.mouse_exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;

    hr = mouse_device_->SetCooperativeLevel(hwnd, mouse_flags);
    if (FAILED(hr)) {
        return false;
    }

    // Acquire devices
    keyboard_device_->Acquire();
    mouse_device_->Acquire();

    return true;
}

void DirectInput8Input::Shutdown() {
    if (keyboard_device_) {
        keyboard_device_->Unacquire();
        keyboard_device_->Release();
        keyboard_device_ = nullptr;
    }
    if (mouse_device_) {
        mouse_device_->Unacquire();
        mouse_device_->Release();
        mouse_device_ = nullptr;
    }
    if (di_) {
        di_->Release();
        di_ = nullptr;
    }

    std::memset(keys_, 0, sizeof(keys_));
    std::memset(prev_keys_, 0, sizeof(prev_keys_));
    std::memset(&mouse_, 0, sizeof(mouse_));
    std::memset(&prev_mouse_, 0, sizeof(prev_mouse_));
}

void DirectInput8Input::Update() {
    // Save previous state for edge detection
    std::memcpy(prev_keys_, keys_, sizeof(keys_));
    prev_mouse_ = mouse_;

    // Poll devices
    PollKeyboard();
    PollMouse();
}

void DirectInput8Input::PollKeyboard() {
    if (!keyboard_device_) {
        return;
    }

    HRESULT hr = keyboard_device_->GetDeviceState(sizeof(keys_), keys_);
    if (FAILED(hr)) {
        // Try to reacquire on failure
        keyboard_device_->Acquire();
        hr = keyboard_device_->GetDeviceState(sizeof(keys_), keys_);
    }

    if (FAILED(hr)) {
        // Clear state on persistent failure
        std::memset(keys_, 0, sizeof(keys_));
    }
}

void DirectInput8Input::PollMouse() {
    if (!mouse_device_) {
        return;
    }

    HRESULT hr = mouse_device_->GetDeviceState(sizeof(mouse_), &mouse_);
    if (FAILED(hr)) {
        // Try to reacquire on failure
        mouse_device_->Acquire();
        hr = mouse_device_->GetDeviceState(sizeof(mouse_), &mouse_);
    }

    if (FAILED(hr)) {
        // Clear state on persistent failure
        std::memset(&mouse_, 0, sizeof(mouse_));
    }
}

bool DirectInput8Input::KeyDown(std::uint8_t dik) const noexcept {
    return (keys_[dik] & 0x80) != 0;
}

bool DirectInput8Input::KeyPressed(std::uint8_t dik) const noexcept {
    const bool now = (keys_[dik] & 0x80) != 0;
    const bool prev = (prev_keys_[dik] & 0x80) != 0;
    return now && !prev;
}

bool DirectInput8Input::KeyReleased(std::uint8_t dik) const noexcept {
    const bool now = (keys_[dik] & 0x80) != 0;
    const bool prev = (prev_keys_[dik] & 0x80) != 0;
    return !now && prev;
}

bool DirectInput8Input::MouseButtonDown(int button) const noexcept {
    if (button < 0 || button >= 8) {
        return false;
    }
    return (mouse_.rgbButtons[button] & 0x80) != 0;
}

}  // namespace input
}  // namespace dnf
