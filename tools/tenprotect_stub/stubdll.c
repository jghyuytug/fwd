// Minimal 32-bit stub DLL for missing anti-cheat dependencies.
// Build (WSL mingw):
//   i686-w64-mingw32-gcc -O2 -m32 -shared -o TerSafe.dll stubdll.c -Wl,--enable-stdcall-fixup
//   i686-w64-mingw32-gcc -O2 -m32 -shared -o bdcap32.dll stubdll.c -Wl,--enable-stdcall-fixup

#include <windows.h>

__attribute__((dllexport)) void __cdecl StubDll_Ping(void) {}

typedef int(__stdcall *StubMethod0_t)(void);

static int __stdcall StubMethod0(void) {
    return 0;
}

static void *g_stub_vtbl[32] = {
    (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0,
    (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0,
    (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0,
    (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0,
    (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0, (void *)StubMethod0,
    (void *)StubMethod0, (void *)StubMethod0,
};

typedef struct StubObj {
    void **vtbl;
} StubObj;

static StubObj g_stub_obj = { g_stub_vtbl };

__attribute__((dllexport)) void *__cdecl CreateObj(void) {
    return &g_stub_obj;
}

__attribute__((dllexport)) void *__cdecl CreateBandiCapture(void) {
    // Some clients treat this return value as a callable entrypoint; return executable code.
    return (void *)StubMethod0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    (void)hinst;
    (void)reason;
    (void)reserved;
    return TRUE;
}
