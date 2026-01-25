#include "dnf/core/win32_window.h"
#include <cstring>

namespace dnf {
namespace core {

namespace {
constexpr const wchar_t* kWindowClassName = L"DnfClientWindowClass";

DWORD ComputeWindowStyle(bool resizable) {
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    if (resizable) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }
    return style;
}
}  // namespace

Win32Window::~Win32Window() {
    Destroy();
}

bool Win32Window::Create(HINSTANCE instance, const WindowCreateInfo& ci) {
    instance_ = instance;

    // Register window class
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &Win32Window::StaticWndProc;
    wc.hInstance = instance_;
    wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClassName;

    if (!::RegisterClassExW(&wc)) {
        const DWORD err = ::GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    // Calculate window size including frame
    RECT r{0, 0, ci.width, ci.height};
    const DWORD style = ComputeWindowStyle(ci.resizable);
    ::AdjustWindowRect(&r, style, FALSE);

    // Create window
    hwnd_ = ::CreateWindowExW(
        0,
        kWindowClassName,
        ci.title,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        nullptr,
        nullptr,
        instance_,
        this);  // Pass 'this' as lpParam

    return hwnd_ != nullptr;
}

void Win32Window::Destroy() {
    if (hwnd_) {
        ::DestroyWindow(hwnd_);
        hwnd_ = nullptr;  // Idempotent - safe to call multiple times
    }
}

void Win32Window::Show(int nCmdShow) {
    if (!hwnd_) {
        return;
    }
    ::ShowWindow(hwnd_, nCmdShow);
    ::UpdateWindow(hwnd_);
}

bool Win32Window::PumpMessages() {
    MSG msg{};
    while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
    return true;
}

LRESULT CALLBACK Win32Window::StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    Win32Window* self = nullptr;

    if (msg == WM_NCCREATE) {
        // Store 'this' pointer in window user data
        const auto* cs = reinterpret_cast<const CREATESTRUCTW*>(lparam);
        self = reinterpret_cast<Win32Window*>(cs->lpCreateParams);
        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hwnd_ = hwnd;
    } else {
        // Retrieve 'this' pointer from window user data
        self = reinterpret_cast<Win32Window*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (self) {
        return self->WndProc(msg, wparam, lparam);
    }

    return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}

LRESULT Win32Window::WndProc(UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE: {
            if (sink_) {
                sink_->OnCloseRequested();
            }
            // Let DefWindowProc handle the close (will send WM_DESTROY)
            return ::DefWindowProcW(hwnd_, msg, wparam, lparam);
        }
        case WM_SIZE: {
            const bool minimized = (wparam == SIZE_MINIMIZED);
            const std::uint32_t w = static_cast<std::uint32_t>(LOWORD(lparam));
            const std::uint32_t h = static_cast<std::uint32_t>(HIWORD(lparam));
            if (sink_) {
                sink_->OnResize(w, h, minimized);
            }
            return 0;
        }
        case WM_DESTROY: {
            ::PostQuitMessage(0);
            return 0;
        }
        case WM_NCDESTROY: {
            // Final message - clear window association
            ::SetWindowLongPtrW(hwnd_, GWLP_USERDATA, 0);
            hwnd_ = nullptr;
            return ::DefWindowProcW(hwnd_, msg, wparam, lparam);
        }
        default:
            break;
    }

    return ::DefWindowProcW(hwnd_, msg, wparam, lparam);
}

}  // namespace core
}  // namespace dnf
