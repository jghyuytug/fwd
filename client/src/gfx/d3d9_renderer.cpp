#include "dnf/gfx/d3d9_renderer.h"

namespace dnf {
namespace gfx {

D3D9Renderer::~D3D9Renderer() {
    Shutdown();
}

bool D3D9Renderer::Initialize(HWND hwnd, std::uint32_t width, std::uint32_t height, const RendererCreateInfo& ci) {
    backbuffer_w_ = width;
    backbuffer_h_ = height;

    // Create Direct3D9 object
    d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d_) {
        return false;
    }

    // Setup present parameters
    pp_ = {};
    pp_.Windowed = ci.windowed ? TRUE : FALSE;
    pp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp_.BackBufferFormat = ci.windowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
    pp_.BackBufferCount = 1;
    pp_.BackBufferWidth = backbuffer_w_;
    pp_.BackBufferHeight = backbuffer_h_;
    pp_.hDeviceWindow = hwnd;
    pp_.EnableAutoDepthStencil = TRUE;
    pp_.AutoDepthStencilFormat = D3DFMT_D16;
    pp_.PresentationInterval = ci.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    // Try hardware vertex processing first, fallback to software
    const DWORD flags_try[] = {
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING
    };

    for (DWORD flags : flags_try) {
        HRESULT hr = d3d_->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
            flags,
            &pp_,
            &device_);

        if (SUCCEEDED(hr)) {
            device_lost_ = false;
            pending_reset_ = false;
            return true;
        }
    }

    return false;
}

void D3D9Renderer::Shutdown() {
    if (device_) {
        device_->Release();
        device_ = nullptr;
    }
    if (d3d_) {
        d3d_->Release();
        d3d_ = nullptr;
    }
    pp_ = {};
    device_lost_ = false;
    pending_reset_ = false;
    minimized_ = false;
    backbuffer_w_ = 0;
    backbuffer_h_ = 0;
}

void D3D9Renderer::OnResize(std::uint32_t width, std::uint32_t height, bool minimized) {
    minimized_ = minimized;
    backbuffer_w_ = width;
    backbuffer_h_ = height;

    if (minimized_) {
        return;  // Skip reset when minimized
    }

    // WM_SIZE can report 0x0 during minimize/transition; don't arm a reset until size is valid
    if (width == 0 || height == 0) {
        return;
    }

    pending_reset_ = true;  // Defer reset to render path
}

void D3D9Renderer::UpdateLostState() {
    if (!device_) {
        device_lost_ = true;
        return;
    }

    const HRESULT hr = device_->TestCooperativeLevel();
    if (hr == D3D_OK) {
        device_lost_ = false;
        return;
    }

    if (hr == D3DERR_DEVICELOST) {
        device_lost_ = true;
        return;
    }

    if (hr == D3DERR_DEVICENOTRESET) {
        device_lost_ = true;
        pending_reset_ = true;
        return;
    }

    // Unknown/driver-internal errors: treat as lost, but don't spam Reset() attempts
    device_lost_ = true;
    pending_reset_ = false;
}

bool D3D9Renderer::ResetDevice(std::uint32_t width, std::uint32_t height) {
    if (!device_) {
        return false;
    }
    if (width == 0 || height == 0) {
        return false;
    }

    pp_.BackBufferWidth = width;
    pp_.BackBufferHeight = height;

    const HRESULT hr = device_->Reset(&pp_);
    if (FAILED(hr)) {
        return false;
    }

    device_lost_ = false;
    pending_reset_ = false;
    return true;
}

bool D3D9Renderer::BeginFrame(D3DCOLOR clear_color) {
    if (minimized_) {
        return false;
    }
    if (!device_) {
        return false;
    }

    UpdateLostState();
    if (pending_reset_) {
        // Only attempt Reset when the device explicitly reports it is ready
        const HRESULT hr = device_->TestCooperativeLevel();
        if (hr == D3DERR_DEVICENOTRESET) {
            (void)ResetDevice(backbuffer_w_, backbuffer_h_);
            // Future-proofing: Reset wipes device state; reapply defaults here when you add rendering state
            // ApplyDefaultStates();
        }

        UpdateLostState();
    }

    if (device_lost_) {
        return false;
    }

    device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_color, 1.0f, 0);
    return SUCCEEDED(device_->BeginScene());
}

void D3D9Renderer::EndFrame() {
    if (!device_ || minimized_ || device_lost_) {
        return;
    }

    device_->EndScene();
    const HRESULT hr = device_->Present(nullptr, nullptr, nullptr, nullptr);
    if (hr == D3DERR_DEVICELOST) {
        device_lost_ = true;
    }
}

}  // namespace gfx
}  // namespace dnf
