#include "dnf/app/application.h"

namespace dnf {
namespace app {

void Application::WindowEventSink::OnCloseRequested() {
    if (app_) {
        app_->RequestExit();
    }
}

void Application::WindowEventSink::OnResize(std::uint32_t width, std::uint32_t height, bool minimized) {
    if (app_) {
        app_->renderer_.OnResize(width, height, minimized);
    }
}

Application::Application()
    : window_sink_(this) {
}

Application::~Application() {
    Shutdown();
}

void Application::SetLastError(const wchar_t* msg) {
    last_error_ = msg ? msg : L"";
}

void Application::Shutdown() noexcept {
    running_ = false;

    if (initialized_ && delegate_) {
        delegate_->OnShutdown(*this);
    }

    delegate_ = nullptr;
    initialized_ = false;

    window_.SetEventSink(nullptr);
    input_.Shutdown();
    renderer_.Shutdown();
    window_.Destroy();
}

bool Application::Initialize(HINSTANCE instance, int nCmdShow, const ApplicationCreateInfo& ci, IApplicationDelegate* delegate) {
    Shutdown();
    last_error_.clear();

    // Create window.
    if (!window_.Create(instance, ci.window)) {
        SetLastError(L"Failed to create window");
        return false;
    }

    // Create renderer.
    const std::uint32_t w = static_cast<std::uint32_t>(ci.window.width);
    const std::uint32_t h = static_cast<std::uint32_t>(ci.window.height);
    if (!renderer_.Initialize(window_.hwnd(), w, h, ci.renderer)) {
        SetLastError(L"Failed to initialize renderer");
        window_.Destroy();
        return false;
    }

    // Create input.
    if (!input_.Initialize(instance, window_.hwnd(), ci.input)) {
        SetLastError(L"Failed to initialize input");
        renderer_.Shutdown();
        window_.Destroy();
        return false;
    }

    clear_color_ = ci.clear_color;
    delegate_ = delegate;

    window_.SetEventSink(&window_sink_);

    if (delegate_) {
        if (!delegate_->OnInit(*this)) {
            SetLastError(L"Application delegate initialization failed");
            // Ensure subsystems are torn down on failure.
            delegate_ = nullptr;
            window_.SetEventSink(nullptr);  // Detach sink before teardown
            input_.Shutdown();
            renderer_.Shutdown();
            window_.Destroy();
            return false;
        }
    }

    window_.Show(nCmdShow);

    initialized_ = true;
    return true;
}

int Application::Run(HINSTANCE instance, int nCmdShow, const ApplicationCreateInfo& ci, IApplicationDelegate* delegate) {
    if (!Initialize(instance, nCmdShow, ci, delegate)) {
        return 1;
    }

    running_ = true;
    timer_.Reset();

    while (running_ && window_.PumpMessages()) {
        const float dt = timer_.Tick();

        input_.Update();

        if (delegate_) {
            delegate_->OnUpdate(*this, dt);
        }

        if (!running_) {
            break;
        }

        if (renderer_.BeginFrame(static_cast<D3DCOLOR>(clear_color_))) {
            if (delegate_) {
                delegate_->OnRender(*this);
            }
            renderer_.EndFrame();
        } else {
            // Device lost or minimized - sleep to reduce CPU usage.
            ::Sleep(16);
        }
    }

    Shutdown();
    return 0;
}

}  // namespace app
}  // namespace dnf
