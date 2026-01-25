#pragma once

#include <cstdint>
#include <string>

#include "dnf/platform/win32.h"
#include "dnf/core/frame_timer.h"
#include "dnf/core/traits.h"
#include "dnf/core/win32_window.h"
#include "dnf/gfx/d3d9_renderer.h"
#include "dnf/input/directinput8_input.h"

namespace dnf {
namespace app {

struct ApplicationCreateInfo {
    core::WindowCreateInfo window{};
    gfx::RendererCreateInfo renderer{};
    input::InputCreateInfo input{};

    // ARGB color (opaque blue by default).
    std::uint32_t clear_color = 0xff0000ffu;
};

// Delegate that receives application lifetime + per-frame callbacks.
// Keeps Application focused on orchestration, not game logic.
struct IApplicationDelegate {
    virtual ~IApplicationDelegate() = default;

    virtual bool OnInit(class Application& app) { (void)app; return true; }
    virtual void OnShutdown(class Application& app) { (void)app; }
    virtual void OnUpdate(class Application& app, float dt_seconds) { (void)app; (void)dt_seconds; }
    virtual void OnRender(class Application& app) { (void)app; }
};

class Application final : private dnf::core::NonCopyable, private dnf::core::NonMovable {
public:
    Application();
    ~Application();

    // Full lifetime run: initialize, loop, shutdown. Returns process exit code.
    int Run(HINSTANCE instance, int nCmdShow, const ApplicationCreateInfo& ci, IApplicationDelegate* delegate);

    void RequestExit() noexcept { running_ = false; }

    const std::wstring& last_error() const noexcept { return last_error_; }

    core::Win32Window& window() noexcept { return window_; }
    const core::Win32Window& window() const noexcept { return window_; }

    gfx::D3D9Renderer& renderer() noexcept { return renderer_; }
    const gfx::D3D9Renderer& renderer() const noexcept { return renderer_; }

    input::DirectInput8Input& input() noexcept { return input_; }
    const input::DirectInput8Input& input() const noexcept { return input_; }

private:
    bool Initialize(HINSTANCE instance, int nCmdShow, const ApplicationCreateInfo& ci, IApplicationDelegate* delegate);
    void Shutdown() noexcept;

    void SetLastError(const wchar_t* msg);

    class WindowEventSink final : public dnf::core::IWindowEventSink {
    public:
        explicit WindowEventSink(Application* app) : app_(app) {}
        void OnCloseRequested() override;
        void OnResize(std::uint32_t width, std::uint32_t height, bool minimized) override;

    private:
        Application* app_ = nullptr;  // non-owning
    };

    bool initialized_ = false;
    bool running_ = false;
    std::uint32_t clear_color_ = 0xff0000ffu;

    std::wstring last_error_;

    core::Win32Window window_;
    gfx::D3D9Renderer renderer_;
    input::DirectInput8Input input_;

    WindowEventSink window_sink_;
    core::FrameTimer timer_;
    IApplicationDelegate* delegate_ = nullptr;  // non-owning
};

}  // namespace app
}  // namespace dnf
