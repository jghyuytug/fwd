#pragma once

#include <cstdint>
#include "dnf/platform/win32.h"
#include "dnf/core/traits.h"

namespace dnf {
namespace core {

// Window creation parameters
struct WindowCreateInfo {
    int width = 800;
    int height = 600;
    const wchar_t* title = L"DnfClient";
    bool resizable = true;
};

// Interface for receiving window events
struct IWindowEventSink {
    virtual ~IWindowEventSink() = default;
    virtual void OnCloseRequested() = 0;
    virtual void OnResize(std::uint32_t width, std::uint32_t height, bool minimized) = 0;
};

// Win32 window wrapper with RAII semantics
class Win32Window final : private NonCopyable, private NonMovable {
public:
    Win32Window() = default;
    ~Win32Window();

    // Create and show window
    bool Create(HINSTANCE instance, const WindowCreateInfo& ci);
    void Destroy();
    void Show(int nCmdShow);

    // Process all queued messages. Returns false if WM_QUIT received.
    bool PumpMessages();

    // Set event sink for window events
    void SetEventSink(IWindowEventSink* sink) noexcept { sink_ = sink; }

    // Accessors
    HWND hwnd() const noexcept { return hwnd_; }
    HINSTANCE instance() const noexcept { return instance_; }

private:
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    IWindowEventSink* sink_ = nullptr;  // non-owning pointer
};

}  // namespace core
}  // namespace dnf
