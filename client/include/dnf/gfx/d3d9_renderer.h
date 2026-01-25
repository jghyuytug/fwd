#pragma once

#include <cstdint>
#include "dnf/platform/win32.h"
#include "dnf/core/traits.h"
#include <d3d9.h>

namespace dnf {
namespace gfx {

// Renderer creation parameters
struct RendererCreateInfo {
    bool windowed = true;
    bool vsync = true;
};

// D3D9 renderer with device lost/reset handling
class D3D9Renderer final : private dnf::core::NonCopyable, private dnf::core::NonMovable {
public:
    D3D9Renderer() = default;
    ~D3D9Renderer();

    // Initialize renderer with window handle and dimensions
    bool Initialize(HWND hwnd, std::uint32_t width, std::uint32_t height, const RendererCreateInfo& ci);
    void Shutdown();

    // Handle window resize/minimize events
    void OnResize(std::uint32_t width, std::uint32_t height, bool minimized);

    // Frame rendering - returns false if rendering should be skipped
    bool BeginFrame(D3DCOLOR clear_color);
    void EndFrame();

    // Accessor
    IDirect3DDevice9* device() const noexcept { return device_; }

private:
    bool ResetDevice(std::uint32_t width, std::uint32_t height);
    void UpdateLostState();

    IDirect3D9* d3d_ = nullptr;
    IDirect3DDevice9* device_ = nullptr;
    D3DPRESENT_PARAMETERS pp_ = {};

    std::uint32_t backbuffer_w_ = 0;
    std::uint32_t backbuffer_h_ = 0;
    bool minimized_ = false;
    bool device_lost_ = false;
    bool pending_reset_ = false;
};

}  // namespace gfx
}  // namespace dnf
