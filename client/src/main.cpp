// main.cpp - DNF Client Phase 1: Application Orchestration
// Step 4: Introduce Application that owns Window/Renderer/Input

#include "dnf/platform/win32.h"
#include "dnf/app/application.h"
#include <dinput.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace {

class DnfClientDelegate final : public dnf::app::IApplicationDelegate {
public:
    void OnUpdate(dnf::app::Application& app, float dt_seconds) override {
        (void)dt_seconds;
        if (app.input().KeyPressed(DIK_ESCAPE)) {
            app.RequestExit();
        }
    }
};

}  // namespace

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;

    dnf::app::Application app;
    dnf::app::ApplicationCreateInfo aci{};
    aci.window.width = 800;
    aci.window.height = 600;
    aci.window.title = L"DNF Client - Phase 1 Step 4";
    aci.window.resizable = true;
    aci.renderer.windowed = true;
    aci.renderer.vsync = true;
    aci.input.foreground = true;
    aci.input.mouse_exclusive = false;

    DnfClientDelegate delegate;
    const int rc = app.Run(hInstance, nCmdShow, aci, &delegate);
    if (rc != 0) {
        const wchar_t* msg = app.last_error().empty() ? L"Application failed" : app.last_error().c_str();
        MessageBoxW(nullptr, msg, L"Error", MB_OK);
    }
    return rc;
}
