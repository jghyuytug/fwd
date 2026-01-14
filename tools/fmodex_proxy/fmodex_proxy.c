// Minimal FMOD Ex proxy for a 32-bit client importing C++ symbols.
//
// This DLL is meant to be named "fmodex.dll" and sit next to the client.
// The original FMOD DLL must be renamed to "fmodex_real.dll".

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <string.h>

static HMODULE g_real = NULL;
static HANDLE g_log = INVALID_HANDLE_VALUE;

static int contains_substr_i(const char *hay, const char *needle);
static int addr_in_range(uintptr_t addr, uintptr_t base, SIZE_T size);

typedef int(WINAPI *MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
typedef int(WINAPI *MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
static MessageBoxA_t g_real_MessageBoxA = NULL;
static MessageBoxW_t g_real_MessageBoxW = NULL;

typedef VOID(WINAPI *OutputDebugStringA_t)(LPCSTR);
typedef VOID(WINAPI *OutputDebugStringW_t)(LPCWSTR);
static OutputDebugStringA_t g_real_OutputDebugStringA = NULL;
static OutputDebugStringW_t g_real_OutputDebugStringW = NULL;

typedef VOID(WINAPI *ExitProcess_t)(UINT);
static ExitProcess_t g_real_ExitProcess = NULL;

typedef BOOL(WINAPI *TerminateProcess_t)(HANDLE, UINT);
static TerminateProcess_t g_real_TerminateProcess = NULL;

typedef HMODULE(WINAPI *LoadLibraryA_t)(LPCSTR);
typedef HMODULE(WINAPI *LoadLibraryW_t)(LPCWSTR);
typedef FARPROC(WINAPI *GetProcAddress_t)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI *GetModuleHandleA_t)(LPCSTR);
typedef HMODULE(WINAPI *GetModuleHandleW_t)(LPCWSTR);
static LoadLibraryA_t g_real_LoadLibraryA = NULL;
static LoadLibraryW_t g_real_LoadLibraryW = NULL;
static GetProcAddress_t g_real_GetProcAddress = NULL;
static GetModuleHandleA_t g_real_GetModuleHandleA = NULL;
static GetModuleHandleW_t g_real_GetModuleHandleW = NULL;
static LONG g_modload_log_count = 0;

typedef VOID(WINAPI *RtlExitUserProcess_t)(UINT);
static RtlExitUserProcess_t g_real_RtlExitUserProcess = NULL;

typedef LONG(WINAPI *NtTerminateProcess_t)(HANDLE, LONG);
static NtTerminateProcess_t g_real_NtTerminateProcess = NULL;

typedef USHORT(WINAPI *RtlCaptureStackBackTrace_t)(ULONG, ULONG, PVOID *, PULONG);

typedef HANDLE(WINAPI *CreateFileW_t)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef HANDLE(WINAPI *CreateFileA_t)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
static CreateFileW_t g_real_CreateFileW = NULL;
static CreateFileA_t g_real_CreateFileA = NULL;
static LONG g_file_log_count = 0;

typedef int(WSAAPI *connect_t)(SOCKET, const struct sockaddr *, int);
static connect_t g_real_connect = NULL;

typedef HANDLE(WINAPI *CreateMutexW_t)(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR);
typedef HANDLE(WINAPI *OpenMutexW_t)(DWORD, BOOL, LPCWSTR);
typedef HANDLE(WINAPI *CreateEventW_t)(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCWSTR);
typedef HANDLE(WINAPI *OpenEventW_t)(DWORD, BOOL, LPCWSTR);
typedef HANDLE(WINAPI *CreateFileMappingW_t)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCWSTR);
typedef HANDLE(WINAPI *OpenFileMappingW_t)(DWORD, BOOL, LPCWSTR);
typedef HANDLE(WINAPI *CreateToolhelp32Snapshot_t)(DWORD, DWORD);
typedef HANDLE(WINAPI *OpenProcess_t)(DWORD, BOOL, DWORD);
typedef DWORD(WINAPI *GetFileAttributesW_t)(LPCWSTR);
typedef LPWSTR(WINAPI *GetCommandLineW_t)(VOID);
typedef BOOL(WINAPI *Process32First_t)(HANDLE, LPPROCESSENTRY32);
typedef BOOL(WINAPI *Process32Next_t)(HANDLE, LPPROCESSENTRY32);
typedef BOOL(WINAPI *Process32FirstW_t)(HANDLE, LPPROCESSENTRY32W);
typedef BOOL(WINAPI *Process32NextW_t)(HANDLE, LPPROCESSENTRY32W);
typedef BOOL(WINAPI *Module32First_t)(HANDLE, LPMODULEENTRY32);
typedef BOOL(WINAPI *Module32Next_t)(HANDLE, LPMODULEENTRY32);
typedef BOOL(WINAPI *Module32FirstW_t)(HANDLE, LPMODULEENTRY32W);
typedef BOOL(WINAPI *Module32NextW_t)(HANDLE, LPMODULEENTRY32W);

static CreateMutexW_t g_real_CreateMutexW = NULL;
static OpenMutexW_t g_real_OpenMutexW = NULL;
static CreateEventW_t g_real_CreateEventW = NULL;
static OpenEventW_t g_real_OpenEventW = NULL;
static CreateFileMappingW_t g_real_CreateFileMappingW = NULL;
static OpenFileMappingW_t g_real_OpenFileMappingW = NULL;
static CreateToolhelp32Snapshot_t g_real_CreateToolhelp32Snapshot = NULL;
static OpenProcess_t g_real_OpenProcess = NULL;
static GetFileAttributesW_t g_real_GetFileAttributesW = NULL;
static GetCommandLineW_t g_real_GetCommandLineW = NULL;
static Process32First_t g_real_Process32First = NULL;
static Process32Next_t g_real_Process32Next = NULL;
static Process32FirstW_t g_real_Process32FirstW = NULL;
static Process32NextW_t g_real_Process32NextW = NULL;
static Module32First_t g_real_Module32First = NULL;
static Module32Next_t g_real_Module32Next = NULL;
static Module32FirstW_t g_real_Module32FirstW = NULL;
static Module32NextW_t g_real_Module32NextW = NULL;
static LONG g_ipc_log_count = 0;
static LONG g_proc_enum_log_count = 0;
static LONG g_mod_enum_log_count = 0;
static LONG g_veh_log_count = 0;
static PVOID g_veh_handle = NULL;
static uintptr_t g_stub_tersafe_base = 0;
static SIZE_T g_stub_tersafe_size = 0;
static uintptr_t g_stub_bdcap_base = 0;
static SIZE_T g_stub_bdcap_size = 0;
static uintptr_t g_stub_tersafe2_base = 0;
static SIZE_T g_stub_tersafe2_size = 0;

typedef struct _UNICODE_STRING_DNF {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR *Buffer;
} UNICODE_STRING_DNF;

typedef struct _OBJECT_ATTRIBUTES_DNF {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING_DNF *ObjectName;
    ULONG Attributes;
    void *SecurityDescriptor;
    void *SecurityQualityOfService;
} OBJECT_ATTRIBUTES_DNF;

typedef struct _IO_STATUS_BLOCK_DNF {
    union {
        LONG Status;
        void *Pointer;
    } u;
    ULONG_PTR Information;
} IO_STATUS_BLOCK_DNF;

typedef LONG(WINAPI *NtCreateFile_t)(HANDLE *FileHandle, ULONG DesiredAccess, OBJECT_ATTRIBUTES_DNF *ObjectAttributes,
                                    IO_STATUS_BLOCK_DNF *IoStatusBlock, LARGE_INTEGER *AllocationSize, ULONG FileAttributes,
                                    ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, void *EaBuffer, ULONG EaLength);
static NtCreateFile_t g_real_NtCreateFile = NULL;
static LONG g_ntcreate_log_count = 0;

typedef int (__stdcall *FMOD_System_GetDriverCaps_t)(void *system, int id, uint32_t *caps, int *rate, int *speakermode);
typedef int (__stdcall *FMOD_System_GetDriverName_t)(void *system, int id, char *name, int namelen);
typedef int (__attribute__((thiscall)) *FMOD_System_getVersion_t)(void *system, uint32_t *version);

// This is the older C++ export available in our real fmodex, lacking diskbusy.
typedef int (__attribute__((thiscall)) *FMOD_Sound_getOpenState_old_t)(void *sound, int *openstate, uint32_t *percent, unsigned char *starving);

static FMOD_System_GetDriverCaps_t pSystem_GetDriverCaps = NULL;
static FMOD_System_GetDriverName_t pSystem_GetDriverName = NULL;
static FMOD_System_getVersion_t pSystem_getVersion = NULL;
static FMOD_Sound_getOpenState_old_t pSound_getOpenState_old = NULL;

static void log_line(const char *msg) {
    if (g_log == INVALID_HANDLE_VALUE || !msg) {
        return;
    }
    DWORD written = 0;
    (void)WriteFile(g_log, msg, (DWORD)strlen(msg), &written, NULL);
}

static void ensure_log_opened(void) {
    if (g_log != INVALID_HANDLE_VALUE) {
        return;
    }

    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, path, (DWORD)(sizeof(path) / sizeof(path[0])));
    if (len == 0 || len >= (DWORD)(sizeof(path) / sizeof(path[0]))) {
        return;
    }

    WCHAR *slash = wcsrchr(path, L'\\');
    if (!slash) {
        return;
    }
    slash[1] = '\0';

    WCHAR logpath[MAX_PATH];
    if (lstrcpynW(logpath, path, (int)(sizeof(logpath) / sizeof(logpath[0]))) == NULL) {
        return;
    }
    if (lstrlenW(logpath) + (int)wcslen(L"fmodex_proxy.log") >= (int)(sizeof(logpath) / sizeof(logpath[0]))) {
        return;
    }
    lstrcatW(logpath, L"fmodex_proxy.log");

    g_log = CreateFileW(logpath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (g_log == INVALID_HANDLE_VALUE) {
        return;
    }
    SetFilePointer(g_log, 0, NULL, FILE_END);
}

static int should_hold_exit(void) {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, path, (DWORD)(sizeof(path) / sizeof(path[0])));
    if (len == 0 || len >= (DWORD)(sizeof(path) / sizeof(path[0]))) {
        return 0;
    }

    WCHAR *slash = wcsrchr(path, L'\\');
    if (!slash) {
        return 0;
    }
    slash[1] = L'\0';

    WCHAR holdpath[MAX_PATH];
    if (lstrcpynW(holdpath, path, (int)(sizeof(holdpath) / sizeof(holdpath[0]))) == NULL) {
        return 0;
    }
    if (lstrlenW(holdpath) + (int)wcslen(L"fmodex_hold_exit") >= (int)(sizeof(holdpath) / sizeof(holdpath[0]))) {
        return 0;
    }
    lstrcatW(holdpath, L"fmodex_hold_exit");

    DWORD attr = GetFileAttributesW(holdpath);
    return (attr != INVALID_FILE_ATTRIBUTES) ? 1 : 0;
}

static void ensure_real_loaded(void) {
    if (g_real) {
        return;
    }

    g_real = LoadLibraryA("fmodex_real.dll");
    if (!g_real) {
        ensure_log_opened();
        log_line("[fmodex_proxy] LoadLibraryA(fmodex_real.dll) failed\r\n");
        return;
    }

    pSystem_GetDriverCaps = (FMOD_System_GetDriverCaps_t)GetProcAddress(g_real, "FMOD_System_GetDriverCaps");
    pSystem_GetDriverName = (FMOD_System_GetDriverName_t)GetProcAddress(g_real, "FMOD_System_GetDriverName");
    pSystem_getVersion = (FMOD_System_getVersion_t)GetProcAddress(g_real, "?getVersion@System@FMOD@@QAG?AW4FMOD_RESULT@@PAI@Z");
    pSound_getOpenState_old = (FMOD_Sound_getOpenState_old_t)GetProcAddress(
        g_real, "?getOpenState@Sound@FMOD@@QAG?AW4FMOD_RESULT@@PAW4FMOD_OPENSTATE@@PAIPA_N@Z");
}

static int WINAPI hook_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    ensure_log_opened();
    void *ret0 = __builtin_return_address(0);
    void *ret1 = __builtin_return_address(1);
    log_line("[fmodex_proxy] MessageBoxA:\r\n");
    {
        char buf[160];
        wsprintfA(buf, "  caller0=%p caller1=%p\r\n", ret0, ret1);
        log_line(buf);
    }
    if (lpCaption) {
        log_line("  caption: ");
        log_line(lpCaption);
        log_line("\r\n");
    }
    if (lpText) {
        log_line("  text: ");
        log_line(lpText);
        log_line("\r\n");
    }
    return g_real_MessageBoxA ? g_real_MessageBoxA(hWnd, lpText, lpCaption, uType) : 0;
}

static int WINAPI hook_MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    ensure_log_opened();
    void *ret0 = __builtin_return_address(0);
    void *ret1 = __builtin_return_address(1);
    log_line("[fmodex_proxy] MessageBoxW:\r\n");
    {
        char buf[160];
        wsprintfA(buf, "  caller0=%p caller1=%p\r\n", ret0, ret1);
        log_line(buf);
    }

    // Auto-dismiss known launcher gate dialogs so the process can continue headless.
    if (lpCaption && lstrcmpW(lpCaption, L"Run Error") == 0) {
        log_line("  [auto-dismissed]\r\n");
        return IDOK;
    }
    if (lpCaption) {
        char capA[512];
        capA[0] = '\0';
        WideCharToMultiByte(CP_UTF8, 0, lpCaption, -1, capA, (int)sizeof(capA), NULL, NULL);
        log_line("  caption: ");
        log_line(capA);
        log_line("\r\n");
    }
    if (lpText) {
        char textA[2048];
        textA[0] = '\0';
        WideCharToMultiByte(CP_UTF8, 0, lpText, -1, textA, (int)sizeof(textA), NULL, NULL);
        log_line("  text: ");
        log_line(textA);
        log_line("\r\n");
    }
    return g_real_MessageBoxW ? g_real_MessageBoxW(hWnd, lpText, lpCaption, uType) : 0;
}

static int is_interesting_name_a(const char *s) {
    if (!s || !s[0]) return 0;
    return contains_substr_i(s, "garena") || contains_substr_i(s, "gplus") || contains_substr_i(s, "platform") || contains_substr_i(s, "dnf");
}

static int is_interesting_name_w(const WCHAR *ws) {
    if (!ws || !ws[0]) return 0;
    char s[520];
    s[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, ws, -1, s, (int)sizeof(s), NULL, NULL);
    return is_interesting_name_a(s);
}

static HMODULE WINAPI hook_LoadLibraryA(LPCSTR lpLibFileName) {
    HMODULE h = g_real_LoadLibraryA ? g_real_LoadLibraryA(lpLibFileName) : NULL;
    DWORD err = h ? 0 : GetLastError();
    LONG n = InterlockedIncrement(&g_modload_log_count);
    if ((lpLibFileName && is_interesting_name_a(lpLibFileName)) || (!h && lpLibFileName) || n <= 40) {
        ensure_log_opened();
        char buf[800];
        wsprintfA(buf, "[fmodex_proxy] LoadLibraryA %s err=%lu name=%s\r\n", h ? "OK" : "FAIL", (unsigned long)err,
                  lpLibFileName ? lpLibFileName : "");
        log_line(buf);
    }
    return h;
}

static HMODULE WINAPI hook_LoadLibraryW(LPCWSTR lpLibFileName) {
    HMODULE h = g_real_LoadLibraryW ? g_real_LoadLibraryW(lpLibFileName) : NULL;
    DWORD err = h ? 0 : GetLastError();
    LONG n = InterlockedIncrement(&g_modload_log_count);
    if ((lpLibFileName && is_interesting_name_w(lpLibFileName)) || (!h && lpLibFileName) || n <= 40) {
        ensure_log_opened();
        char nameA[520];
        nameA[0] = '\0';
        if (lpLibFileName) {
            WideCharToMultiByte(CP_UTF8, 0, lpLibFileName, -1, nameA, (int)sizeof(nameA), NULL, NULL);
        }
        char buf[860];
        wsprintfA(buf, "[fmodex_proxy] LoadLibraryW %s err=%lu name=%s\r\n", h ? "OK" : "FAIL", (unsigned long)err, nameA);
        log_line(buf);
    }
    return h;
}

static HMODULE WINAPI hook_GetModuleHandleA(LPCSTR lpModuleName) {
    HMODULE h = g_real_GetModuleHandleA ? g_real_GetModuleHandleA(lpModuleName) : NULL;
    DWORD err = h ? 0 : GetLastError();
    LONG n = InterlockedIncrement(&g_modload_log_count);
    if ((lpModuleName && is_interesting_name_a(lpModuleName)) || (!h && lpModuleName) || n <= 40) {
        ensure_log_opened();
        char buf[800];
        wsprintfA(buf, "[fmodex_proxy] GetModuleHandleA %s err=%lu name=%s\r\n", h ? "OK" : "FAIL", (unsigned long)err,
                  lpModuleName ? lpModuleName : "");
        log_line(buf);
    }
    return h;
}

static HMODULE WINAPI hook_GetModuleHandleW(LPCWSTR lpModuleName) {
    HMODULE h = g_real_GetModuleHandleW ? g_real_GetModuleHandleW(lpModuleName) : NULL;
    DWORD err = h ? 0 : GetLastError();
    LONG n = InterlockedIncrement(&g_modload_log_count);
    if ((lpModuleName && is_interesting_name_w(lpModuleName)) || (!h && lpModuleName) || n <= 40) {
        ensure_log_opened();
        char nameA[520];
        nameA[0] = '\0';
        if (lpModuleName) {
            WideCharToMultiByte(CP_UTF8, 0, lpModuleName, -1, nameA, (int)sizeof(nameA), NULL, NULL);
        }
        char buf[860];
        wsprintfA(buf, "[fmodex_proxy] GetModuleHandleW %s err=%lu name=%s\r\n", h ? "OK" : "FAIL", (unsigned long)err, nameA);
        log_line(buf);
    }
    return h;
}

static FARPROC WINAPI hook_GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    FARPROC fp = g_real_GetProcAddress ? g_real_GetProcAddress(hModule, lpProcName) : NULL;
    DWORD err = fp ? 0 : GetLastError();
    int isOrdinal = (((uintptr_t)lpProcName) >> 16) == 0;
    LONG n = InterlockedIncrement(&g_modload_log_count);
    uintptr_t hm = (uintptr_t)hModule;
    int watch_mod = addr_in_range(hm, g_stub_bdcap_base, g_stub_bdcap_size) || addr_in_range(hm, g_stub_tersafe_base, g_stub_tersafe_size) ||
                    addr_in_range(hm, g_stub_tersafe2_base, g_stub_tersafe2_size);
    int watch_sym = (!isOrdinal && lpProcName && (is_interesting_name_a(lpProcName) || contains_substr_i(lpProcName, "Create") ||
                                                 contains_substr_i(lpProcName, "Band") || contains_substr_i(lpProcName, "Cap")));
    if (!fp || watch_mod || watch_sym || n <= 80) {
        ensure_log_opened();
        char buf[860];
        if (isOrdinal) {
            wsprintfA(buf, "[fmodex_proxy] GetProcAddress %s err=%lu mod=%p ord=%u\r\n", fp ? "OK" : "FAIL", (unsigned long)err, hModule,
                      (unsigned)(uintptr_t)lpProcName);
        } else {
            wsprintfA(buf, "[fmodex_proxy] GetProcAddress %s err=%lu mod=%p name=%s\r\n", fp ? "OK" : "FAIL", (unsigned long)err, hModule,
                      lpProcName ? lpProcName : "");
        }
        log_line(buf);
    }
    return fp;
}

static VOID WINAPI hook_OutputDebugStringA(LPCSTR lpOutputString) {
    ensure_log_opened();
    log_line("[fmodex_proxy] OutputDebugStringA: ");
    if (lpOutputString) {
        log_line(lpOutputString);
    }
    log_line("\r\n");
    if (g_real_OutputDebugStringA) {
        g_real_OutputDebugStringA(lpOutputString);
    }
}

static VOID WINAPI hook_OutputDebugStringW(LPCWSTR lpOutputString) {
    ensure_log_opened();
    log_line("[fmodex_proxy] OutputDebugStringW: ");
    if (lpOutputString) {
        char textA[2048];
        textA[0] = '\0';
        WideCharToMultiByte(CP_UTF8, 0, lpOutputString, -1, textA, (int)sizeof(textA), NULL, NULL);
        log_line(textA);
    }
    log_line("\r\n");
    if (g_real_OutputDebugStringW) {
        g_real_OutputDebugStringW(lpOutputString);
    }
}

static int ends_with_iw(const WCHAR *s, const WCHAR *suffix) {
    if (!s || !suffix) return 0;
    int sl = lstrlenW(s);
    int tl = lstrlenW(suffix);
    if (sl < tl) return 0;
    const WCHAR *p = s + (sl - tl);
    for (int i = 0; i < tl; i++) {
        WCHAR a = p[i];
        WCHAR b = suffix[i];
        if (a >= L'A' && a <= L'Z') a = (WCHAR)(a - L'A' + L'a');
        if (b >= L'A' && b <= L'Z') b = (WCHAR)(b - L'A' + L'a');
        if (a != b) return 0;
    }
    return 1;
}

static int interesting_path_w(const WCHAR *path) {
    return ends_with_iw(path, L".npk") || ends_with_iw(path, L".pvf") || ends_with_iw(path, L".cfg") || ends_with_iw(path, L".ini") ||
           ends_with_iw(path, L".dat") || ends_with_iw(path, L".xml") || ends_with_iw(path, L".bin");
}

static HANDLE WINAPI hook_CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE h = g_real_CreateFileW ? g_real_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                                                       dwFlagsAndAttributes, hTemplateFile)
                                  : INVALID_HANDLE_VALUE;
    if (h == INVALID_HANDLE_VALUE && lpFileName) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND && ends_with_iw(lpFileName, L".zip") && (wcsstr(lpFileName, L"DNF_DUMP") || wcsstr(lpFileName, L"dnf_dump"))) {
            HANDLE h2 = g_real_CreateFileW ? g_real_CreateFileW(lpFileName, GENERIC_READ | GENERIC_WRITE,
                                                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS,
                                                               FILE_ATTRIBUTE_NORMAL, NULL)
                                           : INVALID_HANDLE_VALUE;
            if (h2 != INVALID_HANDLE_VALUE) {
                ensure_log_opened();
                char pathA[2048];
                pathA[0] = '\0';
                WideCharToMultiByte(CP_UTF8, 0, lpFileName, -1, pathA, (int)sizeof(pathA), NULL, NULL);
                char buf[2300];
                wsprintfA(buf, "[fmodex_proxy] CreateFileW created placeholder zip: %s\r\n", pathA);
                log_line(buf);
                SetLastError(0);
                h = h2;
            }
        }
    }
    if (lpFileName) {
        LONG n = InterlockedIncrement(&g_file_log_count);
        if (n <= 50 || (n <= 500 && interesting_path_w(lpFileName))) {
            DWORD err = (h == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
            ensure_log_opened();
            char pathA[2048];
            pathA[0] = '\0';
            WideCharToMultiByte(CP_UTF8, 0, lpFileName, -1, pathA, (int)sizeof(pathA), NULL, NULL);
            char buf[2300];
            wsprintfA(buf, "[fmodex_proxy] CreateFileW %s err=%lu path=%s\r\n", (h == INVALID_HANDLE_VALUE) ? "FAIL" : "OK",
                      (unsigned long)err, pathA);
            log_line(buf);
        }
    }
    return h;
}

static HANDLE WINAPI hook_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE h = g_real_CreateFileA ? g_real_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                                                       dwFlagsAndAttributes, hTemplateFile)
                                  : INVALID_HANDLE_VALUE;
    if (h == INVALID_HANDLE_VALUE && lpFileName) {
        DWORD err = GetLastError();
        size_t n = strlen(lpFileName);
        const char *ext = (n >= 4) ? (lpFileName + (n - 4)) : "";
        if (err == ERROR_FILE_NOT_FOUND && _stricmp(ext, ".zip") == 0 && (strstr(lpFileName, "DNF_DUMP") || strstr(lpFileName, "dnf_dump"))) {
            HANDLE h2 = g_real_CreateFileA ? g_real_CreateFileA(lpFileName, GENERIC_READ | GENERIC_WRITE,
                                                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS,
                                                               FILE_ATTRIBUTE_NORMAL, NULL)
                                           : INVALID_HANDLE_VALUE;
            if (h2 != INVALID_HANDLE_VALUE) {
                ensure_log_opened();
                log_line("[fmodex_proxy] CreateFileA created placeholder zip\r\n");
                SetLastError(0);
                h = h2;
            }
        }
    }
    if (lpFileName) {
        LONG nlog = InterlockedIncrement(&g_file_log_count);
        if (nlog <= 50) {
            DWORD err = (h == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
            ensure_log_opened();
            char buf[2300];
            wsprintfA(buf, "[fmodex_proxy] CreateFileA %s err=%lu path=%s\r\n", (h == INVALID_HANDLE_VALUE) ? "FAIL" : "OK",
                      (unsigned long)err, lpFileName);
            log_line(buf);
        } else if (nlog <= 500) {
            const char *p = lpFileName;
            size_t n = strlen(p);
            if (n >= 4) {
                const char *ext = p + (n - 4);
                if (_stricmp(ext, ".npk") == 0 || _stricmp(ext, ".pvf") == 0 || _stricmp(ext, ".cfg") == 0 || _stricmp(ext, ".ini") == 0 ||
                    _stricmp(ext, ".dat") == 0 || _stricmp(ext, ".xml") == 0 || _stricmp(ext, ".bin") == 0) {
                    DWORD err = (h == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
                    ensure_log_opened();
                    char buf[2300];
                    wsprintfA(buf, "[fmodex_proxy] CreateFileA %s err=%lu path=%s\r\n", (h == INVALID_HANDLE_VALUE) ? "FAIL" : "OK",
                              (unsigned long)err, lpFileName);
                    log_line(buf);
                }
            }
        }
    }
    return h;
}

static int WSAAPI hook_connect(SOCKET s, const struct sockaddr *name, int namelen) {
    int rc = g_real_connect ? g_real_connect(s, name, namelen) : SOCKET_ERROR;
    ensure_log_opened();
    if (name && (name->sa_family == AF_INET) && namelen >= (int)sizeof(struct sockaddr_in)) {
        const struct sockaddr_in *in = (const struct sockaddr_in *)name;
        const char *ip = inet_ntoa(in->sin_addr);
        if (!ip) ip = "0.0.0.0";
        char buf[256];
        wsprintfA(buf, "[fmodex_proxy] connect %s:%u rc=%d\r\n", ip, (unsigned)ntohs(in->sin_port), rc);
        log_line(buf);
    } else {
        log_line("[fmodex_proxy] connect (non-IPv4)\r\n");
    }
    return rc;
}

static void log_wstr(const char *prefix, const WCHAR *s) {
    ensure_log_opened();
    log_line(prefix);
    if (!s) {
        log_line("(null)\r\n");
        return;
    }
    char buf[1024];
    buf[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, s, -1, buf, (int)sizeof(buf), NULL, NULL);
    log_line(buf);
    log_line("\r\n");
}

static HANDLE WINAPI hook_CreateMutexW(LPSECURITY_ATTRIBUTES attrs, BOOL initialOwner, LPCWSTR name) {
    HANDLE h = g_real_CreateMutexW ? g_real_CreateMutexW(attrs, initialOwner, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] CreateMutexW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_OpenMutexW(DWORD desiredAccess, BOOL inherit, LPCWSTR name) {
    HANDLE h = g_real_OpenMutexW ? g_real_OpenMutexW(desiredAccess, inherit, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] OpenMutexW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_CreateEventW(LPSECURITY_ATTRIBUTES attrs, BOOL manualReset, BOOL initialState, LPCWSTR name) {
    HANDLE h = g_real_CreateEventW ? g_real_CreateEventW(attrs, manualReset, initialState, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] CreateEventW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_OpenEventW(DWORD desiredAccess, BOOL inherit, LPCWSTR name) {
    HANDLE h = g_real_OpenEventW ? g_real_OpenEventW(desiredAccess, inherit, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] OpenEventW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_CreateFileMappingW(HANDLE file, LPSECURITY_ATTRIBUTES attrs, DWORD protect, DWORD maxSizeHigh, DWORD maxSizeLow,
                                            LPCWSTR name) {
    HANDLE h = g_real_CreateFileMappingW ? g_real_CreateFileMappingW(file, attrs, protect, maxSizeHigh, maxSizeLow, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] CreateFileMappingW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_OpenFileMappingW(DWORD desiredAccess, BOOL inherit, LPCWSTR name) {
    HANDLE h = g_real_OpenFileMappingW ? g_real_OpenFileMappingW(desiredAccess, inherit, name) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        log_wstr("[fmodex_proxy] OpenFileMappingW name=", name);
    }
    return h;
}

static HANDLE WINAPI hook_CreateToolhelp32Snapshot(DWORD flags, DWORD pid) {
    HANDLE h = g_real_CreateToolhelp32Snapshot ? g_real_CreateToolhelp32Snapshot(flags, pid) : INVALID_HANDLE_VALUE;
    if (InterlockedIncrement(&g_ipc_log_count) <= 200) {
        char buf[160];
        wsprintfA(buf, "[fmodex_proxy] CreateToolhelp32Snapshot flags=0x%08X pid=%u\r\n", (unsigned)flags, (unsigned)pid);
        ensure_log_opened();
        log_line(buf);
    }
    return h;
}

static HANDLE WINAPI hook_OpenProcess(DWORD desiredAccess, BOOL inherit, DWORD pid) {
    HANDLE h = g_real_OpenProcess ? g_real_OpenProcess(desiredAccess, inherit, pid) : NULL;
    if (InterlockedIncrement(&g_ipc_log_count) <= 300) {
        char buf[200];
        wsprintfA(buf, "[fmodex_proxy] OpenProcess access=0x%08X pid=%u -> %p\r\n", (unsigned)desiredAccess, (unsigned)pid, h);
        ensure_log_opened();
        log_line(buf);
    }
    return h;
}

static DWORD WINAPI hook_GetFileAttributesW(LPCWSTR path) {
    DWORD attr = g_real_GetFileAttributesW ? g_real_GetFileAttributesW(path) : INVALID_FILE_ATTRIBUTES;
    if (path && InterlockedIncrement(&g_ipc_log_count) <= 250) {
        // Only log paths that look like launcher/protection checks.
        if (wcsstr(path, L"Garena") || wcsstr(path, L"garena") || ends_with_iw(path, L".exe") || ends_with_iw(path, L".dll") ||
            ends_with_iw(path, L".dat") || ends_with_iw(path, L".xml") || ends_with_iw(path, L".ini")) {
            ensure_log_opened();
            char pA[1024];
            pA[0] = '\0';
            WideCharToMultiByte(CP_UTF8, 0, path, -1, pA, (int)sizeof(pA), NULL, NULL);
            char buf[1400];
            wsprintfA(buf, "[fmodex_proxy] GetFileAttributesW attr=0x%08X path=%s\r\n", (unsigned)attr, pA);
            log_line(buf);
        }
    }
    return attr;
}

static LPWSTR WINAPI hook_GetCommandLineW(VOID) {
    LPWSTR s = g_real_GetCommandLineW ? g_real_GetCommandLineW() : NULL;
    if (s && InterlockedIncrement(&g_ipc_log_count) <= 50) {
        log_wstr("[fmodex_proxy] GetCommandLineW=", s);
    }
    return s;
}

static int contains_substr_i(const char *hay, const char *needle) {
    if (!hay || !needle) return 0;
    size_t nl = strlen(needle);
    if (!nl) return 0;
    for (const char *p = hay; *p; p++) {
        size_t i = 0;
        for (; i < nl; i++) {
            char a = p[i];
            char b = needle[i];
            if (!a) break;
            if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
            if (a != b) break;
        }
        if (i == nl) return 1;
    }
    return 0;
}

static void log_proc_entry(const PROCESSENTRY32 *pe) {
    if (!pe) return;
    const char *name = pe->szExeFile;
    if (!name || !name[0]) return;
    if (!contains_substr_i(name, "garena") && !contains_substr_i(name, "gplus") && !contains_substr_i(name, "dnf")) {
        return;
    }
    ensure_log_opened();
    char buf[320];
    wsprintfA(buf, "[fmodex_proxy] ProcEnum pid=%u exe=%s\r\n", (unsigned)pe->th32ProcessID, name);
    log_line(buf);
}

static BOOL WINAPI hook_Process32First(HANDLE snapshot, LPPROCESSENTRY32 pe) {
    BOOL ok = g_real_Process32First ? g_real_Process32First(snapshot, pe) : FALSE;
    if (ok && pe && InterlockedIncrement(&g_proc_enum_log_count) <= 200) {
        log_proc_entry(pe);
    }
    return ok;
}

static BOOL WINAPI hook_Process32Next(HANDLE snapshot, LPPROCESSENTRY32 pe) {
    BOOL ok = g_real_Process32Next ? g_real_Process32Next(snapshot, pe) : FALSE;
    if (ok && pe && InterlockedIncrement(&g_proc_enum_log_count) <= 200) {
        log_proc_entry(pe);
    }
    return ok;
}

static void log_proc_entry_w(const PROCESSENTRY32W *pe) {
    if (!pe) return;
    const WCHAR *wname = pe->szExeFile;
    if (!wname || !wname[0]) return;
    char nameA[300];
    nameA[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, wname, -1, nameA, (int)sizeof(nameA), NULL, NULL);
    if (!nameA[0]) return;
    if (!contains_substr_i(nameA, "garena") && !contains_substr_i(nameA, "gplus") && !contains_substr_i(nameA, "dnf")) {
        return;
    }
    ensure_log_opened();
    char buf[360];
    wsprintfA(buf, "[fmodex_proxy] ProcEnumW pid=%u exe=%s\r\n", (unsigned)pe->th32ProcessID, nameA);
    log_line(buf);
}

static BOOL WINAPI hook_Process32FirstW(HANDLE snapshot, LPPROCESSENTRY32W pe) {
    BOOL ok = g_real_Process32FirstW ? g_real_Process32FirstW(snapshot, pe) : FALSE;
    if (ok && pe && InterlockedIncrement(&g_proc_enum_log_count) <= 240) {
        log_proc_entry_w(pe);
    }
    return ok;
}

static BOOL WINAPI hook_Process32NextW(HANDLE snapshot, LPPROCESSENTRY32W pe) {
    BOOL ok = g_real_Process32NextW ? g_real_Process32NextW(snapshot, pe) : FALSE;
    if (ok && pe && InterlockedIncrement(&g_proc_enum_log_count) <= 240) {
        log_proc_entry_w(pe);
    }
    return ok;
}

static void log_module_entry(const MODULEENTRY32 *me) {
    if (!me) return;
    const char *mod = me->szModule;
    const char *path = me->szExePath;
    if (!mod || !mod[0]) return;
    ensure_log_opened();
    char buf[640];
    wsprintfA(buf, "[fmodex_proxy] ModEnum pid=%u base=%p size=0x%08X mod=%s path=%s\r\n", (unsigned)me->th32ProcessID,
              me->modBaseAddr, (unsigned)me->modBaseSize, mod, path ? path : "");
    log_line(buf);
}

static BOOL WINAPI hook_Module32First(HANDLE snapshot, LPMODULEENTRY32 me) {
    BOOL ok = g_real_Module32First ? g_real_Module32First(snapshot, me) : FALSE;
    if (ok && me && InterlockedIncrement(&g_mod_enum_log_count) <= 120) {
        log_module_entry(me);
    }
    return ok;
}

static BOOL WINAPI hook_Module32Next(HANDLE snapshot, LPMODULEENTRY32 me) {
    BOOL ok = g_real_Module32Next ? g_real_Module32Next(snapshot, me) : FALSE;
    if (ok && me && InterlockedIncrement(&g_mod_enum_log_count) <= 120) {
        log_module_entry(me);
    }
    return ok;
}

static void log_module_entry_w(const MODULEENTRY32W *me) {
    if (!me) return;
    const WCHAR *wmod = me->szModule;
    const WCHAR *wpath = me->szExePath;
    if (!wmod || !wmod[0]) return;
    char modA[260];
    char pathA[520];
    modA[0] = '\0';
    pathA[0] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, wmod, -1, modA, (int)sizeof(modA), NULL, NULL);
    if (wpath) {
        WideCharToMultiByte(CP_UTF8, 0, wpath, -1, pathA, (int)sizeof(pathA), NULL, NULL);
    }
    ensure_log_opened();
    char buf[820];
    wsprintfA(buf, "[fmodex_proxy] ModEnumW pid=%u base=%p size=0x%08X mod=%s path=%s\r\n", (unsigned)me->th32ProcessID,
              me->modBaseAddr, (unsigned)me->modBaseSize, modA, pathA);
    log_line(buf);
}

static BOOL WINAPI hook_Module32FirstW(HANDLE snapshot, LPMODULEENTRY32W me) {
    BOOL ok = g_real_Module32FirstW ? g_real_Module32FirstW(snapshot, me) : FALSE;
    if (ok && me && InterlockedIncrement(&g_mod_enum_log_count) <= 160) {
        log_module_entry_w(me);
    }
    return ok;
}

static BOOL WINAPI hook_Module32NextW(HANDLE snapshot, LPMODULEENTRY32W me) {
    BOOL ok = g_real_Module32NextW ? g_real_Module32NextW(snapshot, me) : FALSE;
    if (ok && me && InterlockedIncrement(&g_mod_enum_log_count) <= 160) {
        log_module_entry_w(me);
    }
    return ok;
}

static LONG WINAPI hook_NtCreateFile(HANDLE *FileHandle, ULONG DesiredAccess, OBJECT_ATTRIBUTES_DNF *ObjectAttributes,
                                    IO_STATUS_BLOCK_DNF *IoStatusBlock, LARGE_INTEGER *AllocationSize, ULONG FileAttributes,
                                    ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, void *EaBuffer, ULONG EaLength) {
    LONG status = g_real_NtCreateFile ? g_real_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize,
                                                           FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength)
                                      : (LONG)0xC0000001;
    if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer &&
        InterlockedIncrement(&g_ntcreate_log_count) <= 200) {
        UNICODE_STRING_DNF *name = ObjectAttributes->ObjectName;
        int wlen = (int)(name->Length / sizeof(WCHAR));
        if (wlen > 0 && wlen < 2048) {
            WCHAR tmp[2048];
            int copy = wlen;
            if (copy > (int)(sizeof(tmp) / sizeof(tmp[0])) - 1) copy = (int)(sizeof(tmp) / sizeof(tmp[0])) - 1;
            memcpy(tmp, name->Buffer, (size_t)copy * sizeof(WCHAR));
            tmp[copy] = L'\0';

            // Log only likely-interesting paths to keep noise down.
            if (wcsstr(tmp, L"Garena") || wcsstr(tmp, L"garena") || wcsstr(tmp, L"pipe") || ends_with_iw(tmp, L".xml") ||
                ends_with_iw(tmp, L".cfg") || ends_with_iw(tmp, L".ini") || ends_with_iw(tmp, L".pvf") || ends_with_iw(tmp, L".npk")) {
                ensure_log_opened();
                char pathA[2300];
                pathA[0] = '\0';
                WideCharToMultiByte(CP_UTF8, 0, tmp, -1, pathA, (int)sizeof(pathA), NULL, NULL);
                char buf[2600];
                wsprintfA(buf, "[fmodex_proxy] NtCreateFile status=0x%08X path=%s\r\n", (unsigned)status, pathA);
                log_line(buf);
            }
        }
    }
    return status;
}

static VOID WINAPI hook_ExitProcess(UINT uExitCode) {
    ensure_log_opened();
    void *ret = __builtin_return_address(0);
    char buf[160];
    wsprintfA(buf, "[fmodex_proxy] ExitProcess(code=%u) caller=%p\r\n", (unsigned)uExitCode, ret);
    log_line(buf);
    if (g_real_ExitProcess) {
        g_real_ExitProcess(uExitCode);
    }
    for (;;) {
        Sleep(1000);
    }
}

static BOOL WINAPI hook_TerminateProcess(HANDLE hProcess, UINT uExitCode) {
    ensure_log_opened();
    void *ret = __builtin_return_address(0);
    char buf[200];
    wsprintfA(buf, "[fmodex_proxy] TerminateProcess(h=%p code=%u) caller=%p\r\n", hProcess, (unsigned)uExitCode, ret);
    log_line(buf);
    return g_real_TerminateProcess ? g_real_TerminateProcess(hProcess, uExitCode) : FALSE;
}

static VOID WINAPI hook_RtlExitUserProcess(UINT exitStatus) {
    ensure_log_opened();
    void *ret0 = __builtin_return_address(0);
    void *ret1 = __builtin_return_address(1);
    char buf[200];
    wsprintfA(buf, "[fmodex_proxy] RtlExitUserProcess(status=%u) caller0=%p caller1=%p\r\n", (unsigned)exitStatus, ret0, ret1);
    log_line(buf);

    PVOID frames[24];
    USHORT captured = 0;
    RtlCaptureStackBackTrace_t cap = (RtlCaptureStackBackTrace_t)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlCaptureStackBackTrace");
    if (cap) {
        captured = cap(0, (ULONG)(sizeof(frames) / sizeof(frames[0])), frames, NULL);
    }
    if (captured) {
        log_line("[fmodex_proxy] stack:\r\n");
        for (USHORT i = 0; i < captured; i++) {
            char line[64];
            wsprintfA(line, "  #%02u %p\r\n", (unsigned)i, frames[i]);
            log_line(line);
        }
    }

    if (!should_hold_exit() && g_real_RtlExitUserProcess) {
        g_real_RtlExitUserProcess(exitStatus);
    }
    for (;;) Sleep(1000);
}

static LONG WINAPI hook_NtTerminateProcess(HANDLE hProcess, LONG exitStatus) {
    ensure_log_opened();
    void *ret = __builtin_return_address(0);
    char buf[220];
    wsprintfA(buf, "[fmodex_proxy] NtTerminateProcess(h=%p status=0x%08X) caller=%p\r\n", hProcess, (unsigned)exitStatus, ret);
    log_line(buf);
    return g_real_NtTerminateProcess ? g_real_NtTerminateProcess(hProcess, exitStatus) : (LONG)0xC0000001;
}

static SIZE_T get_pe_image_size(HMODULE module) {
    if (!module) return 0;
    uint8_t *base = (uint8_t *)module;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    return (SIZE_T)nt->OptionalHeader.SizeOfImage;
}

static int addr_in_range(uintptr_t addr, uintptr_t base, SIZE_T size) {
    if (!base || !size) return 0;
    return addr >= base && addr < (base + (uintptr_t)size);
}

static int is_interesting_exception(DWORD code) {
    switch (code) {
    case 0xC0000005: // ACCESS_VIOLATION
    case 0xC000001D: // ILLEGAL_INSTRUCTION
    case 0xC0000409: // STACK_BUFFER_OVERRUN / FAST_FAIL
    case 0xC0000374: // HEAP_CORRUPTION
    case 0xC0000135: // DLL_NOT_FOUND
        return 1;
    default:
        return 0;
    }
}

static LONG WINAPI veh_handler(PEXCEPTION_POINTERS ep) {
    if (!ep || !ep->ExceptionRecord) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if (!is_interesting_exception(code)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (InterlockedIncrement(&g_veh_log_count) > 20) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    ensure_log_opened();
    void *addr = ep->ExceptionRecord->ExceptionAddress;
    char buf[200];
    uintptr_t base = (uintptr_t)GetModuleHandleW(NULL);
    uintptr_t a = (uintptr_t)addr;
    unsigned rva = (a >= base) ? (unsigned)(a - base) : 0u;
    wsprintfA(buf, "[fmodex_proxy] VEH exception code=0x%08X addr=%p base=%p rva=0x%08X\r\n", (unsigned)code, addr, (void *)base, rva);
    log_line(buf);

    if (code == 0xC0000005 && ep->ExceptionRecord->NumberParameters >= 2) {
        ULONG_PTR op = ep->ExceptionRecord->ExceptionInformation[0];
        ULONG_PTR bad = ep->ExceptionRecord->ExceptionInformation[1];
        char buf2[220];
        wsprintfA(buf2, "[fmodex_proxy] VEH AV op=%Iu bad=%p\r\n", (size_t)op, (void *)bad);
        log_line(buf2);

        // If this is an execute fault on committed memory, try to make the page executable and continue.
        // This helps when packers/anti-cheat JIT code into RW pages without calling VirtualProtect under some setups.
        if (op == 8 && bad) {
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery((void *)bad, &mbi, sizeof(mbi)) == sizeof(mbi) && mbi.State == MEM_COMMIT) {
                DWORD oldProt = 0;
                if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProt)) {
                    char buf3[240];
                    wsprintfA(buf3, "[fmodex_proxy] VEH made executable: base=%p size=0x%Ix old=0x%08X\r\n", mbi.BaseAddress,
                              (size_t)mbi.RegionSize, (unsigned)oldProt);
                    log_line(buf3);
                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }

        if (op == 1 && bad) {
            uintptr_t b = (uintptr_t)bad;
            if (addr_in_range(b, g_stub_tersafe_base, g_stub_tersafe_size) || addr_in_range(b, g_stub_bdcap_base, g_stub_bdcap_size)) {
                MEMORY_BASIC_INFORMATION mbi;
                if (VirtualQuery((void *)bad, &mbi, sizeof(mbi)) == sizeof(mbi) && mbi.State == MEM_COMMIT) {
                    DWORD oldProt = 0;
                    if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProt)) {
                        char buf3[240];
                        wsprintfA(buf3, "[fmodex_proxy] VEH made writable: base=%p size=0x%Ix old=0x%08X\r\n", mbi.BaseAddress,
                                  (size_t)mbi.RegionSize, (unsigned)oldProt);
                        log_line(buf3);
                        return EXCEPTION_CONTINUE_EXECUTION;
                    }
                }
            }
        }
    }

    PVOID frames[24];
    USHORT captured = 0;
    RtlCaptureStackBackTrace_t cap = (RtlCaptureStackBackTrace_t)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlCaptureStackBackTrace");
    if (cap) {
        captured = cap(0, (ULONG)(sizeof(frames) / sizeof(frames[0])), frames, NULL);
    }
    if (captured) {
        log_line("[fmodex_proxy] VEH stack:\r\n");
        for (USHORT i = 0; i < captured; i++) {
            char line[64];
            wsprintfA(line, "  #%02u %p\r\n", (unsigned)i, frames[i]);
            log_line(line);
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static void patch_iat_import_in_module(HMODULE module, const char *dll_name, const char *func_name, void *hook_fn,
                                       void **orig_fn_out) {
    if (!module) {
        return;
    }

    uint8_t *base = (uint8_t *)module;
    SIZE_T image_size = 0;
    IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER *)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }

    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return;
    }
    image_size = (SIZE_T)nt->OptionalHeader.SizeOfImage;
    if (image_size < 0x1000) {
        return;
    }

    #define RVA_PTR(rva, type) ((type)(base + (SIZE_T)(rva)))
    #define RVA_IN_RANGE(rva, need) ((SIZE_T)(rva) < image_size && (SIZE_T)(need) <= image_size - (SIZE_T)(rva))

    IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress || !dir.Size) {
        return;
    }
    if (!RVA_IN_RANGE(dir.VirtualAddress, sizeof(IMAGE_IMPORT_DESCRIPTOR))) {
        return;
    }

    IMAGE_IMPORT_DESCRIPTOR *imp = RVA_PTR(dir.VirtualAddress, IMAGE_IMPORT_DESCRIPTOR *);
    for (; imp->Name; imp++) {
        if (!RVA_IN_RANGE(imp->Name, 1)) {
            continue;
        }
        const char *name = RVA_PTR(imp->Name, const char *);
        if (!name || lstrcmpiA(name, dll_name) != 0) {
            continue;
        }

        if (!RVA_IN_RANGE(imp->FirstThunk, sizeof(IMAGE_THUNK_DATA))) {
            continue;
        }

        IMAGE_THUNK_DATA *thunk = RVA_PTR(imp->FirstThunk, IMAGE_THUNK_DATA *);

        // Name-based patching requires a valid OriginalFirstThunk (Import Name Table).
        // Some binaries omit it (OriginalFirstThunk==0). In that case, fall back to matching by resolved function address.
        if (imp->OriginalFirstThunk) {
            if (!RVA_IN_RANGE(imp->OriginalFirstThunk, sizeof(IMAGE_THUNK_DATA))) {
                continue;
            }
            IMAGE_THUNK_DATA *orig_thunk = RVA_PTR(imp->OriginalFirstThunk, IMAGE_THUNK_DATA *);
            for (; orig_thunk->u1.AddressOfData; orig_thunk++, thunk++) {
                if (orig_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
                    continue;
                }

                if (!RVA_IN_RANGE(orig_thunk->u1.AddressOfData, sizeof(IMAGE_IMPORT_BY_NAME))) {
                    continue;
                }
                IMAGE_IMPORT_BY_NAME *by_name = RVA_PTR(orig_thunk->u1.AddressOfData, IMAGE_IMPORT_BY_NAME *);
                const char *import_name = (const char *)by_name->Name;
                if (!import_name || lstrcmpA(import_name, func_name) != 0) {
                    continue;
                }

                DWORD old_protect = 0;
                if (!VirtualProtect(&thunk->u1.Function, sizeof(void *), PAGE_READWRITE, &old_protect)) {
                    return;
                }

                if (orig_fn_out && !*orig_fn_out) {
                    *orig_fn_out = (void *)(uintptr_t)thunk->u1.Function;
                }
                thunk->u1.Function = (uintptr_t)hook_fn;

                DWORD tmp = 0;
                VirtualProtect(&thunk->u1.Function, sizeof(void *), old_protect, &tmp);
                return;
            }
        } else {
            HMODULE dep = GetModuleHandleA(dll_name);
            if (!dep) {
                dep = LoadLibraryA(dll_name);
            }
            FARPROC target = dep ? GetProcAddress(dep, func_name) : NULL;
            if (!target) {
                continue;
            }

            for (; thunk->u1.Function; thunk++) {
                if ((void *)(uintptr_t)thunk->u1.Function != (void *)target) {
                    continue;
                }

                DWORD old_protect = 0;
                if (!VirtualProtect(&thunk->u1.Function, sizeof(void *), PAGE_READWRITE, &old_protect)) {
                    return;
                }

                if (orig_fn_out && !*orig_fn_out) {
                    *orig_fn_out = (void *)(uintptr_t)thunk->u1.Function;
                }
                thunk->u1.Function = (uintptr_t)hook_fn;

                DWORD tmp = 0;
                VirtualProtect(&thunk->u1.Function, sizeof(void *), old_protect, &tmp);
                return;
            }
        }
    }

    #undef RVA_PTR
    #undef RVA_IN_RANGE
}

static void patch_iat_import_all_modules(const char *dll_name, const char *func_name, void *hook_fn, void **orig_fn_out) {
    DWORD pid = GetCurrentProcessId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (snap == INVALID_HANDLE_VALUE) {
        return;
    }

    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    if (Module32First(snap, &me)) {
        do {
            patch_iat_import_in_module(me.hModule, dll_name, func_name, hook_fn, orig_fn_out);
        } while (Module32Next(snap, &me));
    }

    CloseHandle(snap);
}

static DWORD WINAPI install_hooks_thread(LPVOID unused) {
    (void)unused;
    ensure_log_opened();
    log_line("[fmodex_proxy] installing IAT hooks\r\n");

    patch_iat_import_all_modules("user32.dll", "MessageBoxA", (void *)&hook_MessageBoxA, (void **)&g_real_MessageBoxA);
    patch_iat_import_all_modules("user32.dll", "MessageBoxW", (void *)&hook_MessageBoxW, (void **)&g_real_MessageBoxW);
    patch_iat_import_all_modules("kernel32.dll", "OutputDebugStringA", (void *)&hook_OutputDebugStringA,
                                 (void **)&g_real_OutputDebugStringA);
    patch_iat_import_all_modules("kernel32.dll", "OutputDebugStringW", (void *)&hook_OutputDebugStringW,
                                 (void **)&g_real_OutputDebugStringW);
    patch_iat_import_all_modules("kernel32.dll", "ExitProcess", (void *)&hook_ExitProcess, (void **)&g_real_ExitProcess);
    patch_iat_import_all_modules("kernel32.dll", "TerminateProcess", (void *)&hook_TerminateProcess,
                                 (void **)&g_real_TerminateProcess);
    patch_iat_import_all_modules("kernel32.dll", "LoadLibraryA", (void *)&hook_LoadLibraryA, (void **)&g_real_LoadLibraryA);
    patch_iat_import_all_modules("kernel32.dll", "LoadLibraryW", (void *)&hook_LoadLibraryW, (void **)&g_real_LoadLibraryW);
    patch_iat_import_all_modules("kernel32.dll", "GetProcAddress", (void *)&hook_GetProcAddress, (void **)&g_real_GetProcAddress);
    patch_iat_import_all_modules("kernel32.dll", "GetModuleHandleA", (void *)&hook_GetModuleHandleA, (void **)&g_real_GetModuleHandleA);
    patch_iat_import_all_modules("kernel32.dll", "GetModuleHandleW", (void *)&hook_GetModuleHandleW, (void **)&g_real_GetModuleHandleW);
    patch_iat_import_all_modules("kernel32.dll", "CreateFileW", (void *)&hook_CreateFileW, (void **)&g_real_CreateFileW);
    patch_iat_import_all_modules("kernel32.dll", "CreateFileA", (void *)&hook_CreateFileA, (void **)&g_real_CreateFileA);
    patch_iat_import_all_modules("kernel32.dll", "CreateMutexW", (void *)&hook_CreateMutexW, (void **)&g_real_CreateMutexW);
    patch_iat_import_all_modules("kernel32.dll", "OpenMutexW", (void *)&hook_OpenMutexW, (void **)&g_real_OpenMutexW);
    patch_iat_import_all_modules("kernel32.dll", "CreateEventW", (void *)&hook_CreateEventW, (void **)&g_real_CreateEventW);
    patch_iat_import_all_modules("kernel32.dll", "OpenEventW", (void *)&hook_OpenEventW, (void **)&g_real_OpenEventW);
    patch_iat_import_all_modules("kernel32.dll", "CreateFileMappingW", (void *)&hook_CreateFileMappingW, (void **)&g_real_CreateFileMappingW);
    patch_iat_import_all_modules("kernel32.dll", "OpenFileMappingW", (void *)&hook_OpenFileMappingW, (void **)&g_real_OpenFileMappingW);
    patch_iat_import_all_modules("kernel32.dll", "CreateToolhelp32Snapshot", (void *)&hook_CreateToolhelp32Snapshot,
                                 (void **)&g_real_CreateToolhelp32Snapshot);
    patch_iat_import_all_modules("kernel32.dll", "OpenProcess", (void *)&hook_OpenProcess, (void **)&g_real_OpenProcess);
    patch_iat_import_all_modules("kernel32.dll", "GetFileAttributesW", (void *)&hook_GetFileAttributesW, (void **)&g_real_GetFileAttributesW);
    patch_iat_import_all_modules("kernel32.dll", "GetCommandLineW", (void *)&hook_GetCommandLineW, (void **)&g_real_GetCommandLineW);
    patch_iat_import_all_modules("kernel32.dll", "Process32First", (void *)&hook_Process32First, (void **)&g_real_Process32First);
    patch_iat_import_all_modules("kernel32.dll", "Process32Next", (void *)&hook_Process32Next, (void **)&g_real_Process32Next);
    patch_iat_import_all_modules("kernel32.dll", "Process32FirstW", (void *)&hook_Process32FirstW, (void **)&g_real_Process32FirstW);
    patch_iat_import_all_modules("kernel32.dll", "Process32NextW", (void *)&hook_Process32NextW, (void **)&g_real_Process32NextW);
    patch_iat_import_all_modules("kernel32.dll", "Module32First", (void *)&hook_Module32First, (void **)&g_real_Module32First);
    patch_iat_import_all_modules("kernel32.dll", "Module32Next", (void *)&hook_Module32Next, (void **)&g_real_Module32Next);
    patch_iat_import_all_modules("kernel32.dll", "Module32FirstW", (void *)&hook_Module32FirstW, (void **)&g_real_Module32FirstW);
    patch_iat_import_all_modules("kernel32.dll", "Module32NextW", (void *)&hook_Module32NextW, (void **)&g_real_Module32NextW);
    patch_iat_import_all_modules("ws2_32.dll", "connect", (void *)&hook_connect, (void **)&g_real_connect);
    patch_iat_import_all_modules("ntdll.dll", "RtlExitUserProcess", (void *)&hook_RtlExitUserProcess,
                                 (void **)&g_real_RtlExitUserProcess);
    patch_iat_import_all_modules("ntdll.dll", "NtCreateFile", (void *)&hook_NtCreateFile, (void **)&g_real_NtCreateFile);
    patch_iat_import_all_modules("ntdll.dll", "NtTerminateProcess", (void *)&hook_NtTerminateProcess,
                                 (void **)&g_real_NtTerminateProcess);

    if (g_real_CreateFileW) {
        char buf[128];
        wsprintfA(buf, "[fmodex_proxy] hooked CreateFileW orig=%p\r\n", g_real_CreateFileW);
        log_line(buf);
    } else {
        log_line("[fmodex_proxy] CreateFileW not hooked\r\n");
    }
    if (g_real_CreateFileA) {
        char buf[128];
        wsprintfA(buf, "[fmodex_proxy] hooked CreateFileA orig=%p\r\n", g_real_CreateFileA);
        log_line(buf);
    } else {
        log_line("[fmodex_proxy] CreateFileA not hooked\r\n");
    }
    if (g_real_connect) {
        char buf[128];
        wsprintfA(buf, "[fmodex_proxy] hooked connect orig=%p\r\n", g_real_connect);
        log_line(buf);
    } else {
        log_line("[fmodex_proxy] connect not hooked\r\n");
    }
    if (g_real_NtCreateFile) {
        char buf[128];
        wsprintfA(buf, "[fmodex_proxy] hooked NtCreateFile orig=%p\r\n", g_real_NtCreateFile);
        log_line(buf);
    } else {
        log_line("[fmodex_proxy] NtCreateFile not hooked\r\n");
    }
    log_line("[fmodex_proxy] IAT hooks install done\r\n");

    return 0;
}

// Exported as:
// ?getVersion@System@FMOD@@QAG?AW4FMOD_RESULT@@PAI@Z
int __attribute__((thiscall)) proxy_getVersion(void *system, uint32_t *version) {
    ensure_real_loaded();

    uint32_t real_ver = 0;
    int real_rc = -1;
    if (pSystem_getVersion) {
        real_rc = pSystem_getVersion(system, &real_ver);
    }

    if (version) {
        *version = 0x0004FFFFu;
    }

    ensure_log_opened();
    char buf[160];
    wsprintfA(buf, "[fmodex_proxy] getVersion real_rc=%d real_ver=0x%08X -> fake=0x%08X\r\n", real_rc, real_ver,
              version ? *version : 0u);
    log_line(buf);

    return 0;
}

// Exported as:
// ?getDriverInfo@System@FMOD@@QAG?AW4FMOD_RESULT@@HPADHPAUFMOD_GUID@@@Z
int __attribute__((thiscall)) proxy_getDriverInfo(void *system, int id, char *name, int namelen, void *guid) {
    ensure_real_loaded();

    if (guid) {
        memset(guid, 0, 16);
    }

    if (!pSystem_GetDriverName) {
        if (name && namelen > 0) {
            name[0] = '\0';
        }
        return 1;
    }

    return pSystem_GetDriverName(system, id, name, namelen);
}

// Exported as:
// ?getOpenState@Sound@FMOD@@QAG?AW4FMOD_RESULT@@PAW4FMOD_OPENSTATE@@PAIPA_N2@Z
int __attribute__((thiscall)) proxy_getOpenState(void *sound, int *openstate, uint32_t *percent, unsigned char *starving,
                                                 unsigned char *diskbusy) {
    ensure_real_loaded();

    int result = 1;
    if (pSound_getOpenState_old) {
        result = pSound_getOpenState_old(sound, openstate, percent, starving);
    } else {
        if (openstate) *openstate = 0;
        if (percent) *percent = 0;
        if (starving) *starving = 0;
    }

    if (diskbusy) {
        *diskbusy = 0;
    }

    return result;
}

// Exported as:
// ?getDriverCaps@System@FMOD@@QAG?AW4FMOD_RESULT@@HPAIPAHPAW4FMOD_SPEAKERMODE@@@Z
int __attribute__((thiscall)) proxy_getDriverCaps(void *system, int id, uint32_t *caps, int *rate, int *speakermode) {
    ensure_real_loaded();

    if (!pSystem_GetDriverCaps) {
        if (caps) *caps = 0;
        if (rate) *rate = 0;
        if (speakermode) *speakermode = 0;
        return 1;
    }

    return pSystem_GetDriverCaps(system, id, caps, rate, speakermode);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    (void)hinst;
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        ensure_log_opened();
        log_line("[fmodex_proxy] attached\r\n");
        DisableThreadLibraryCalls(hinst);

        if (!g_veh_handle) {
            g_veh_handle = AddVectoredExceptionHandler(1, veh_handler);
            if (g_veh_handle) {
                log_line("[fmodex_proxy] VEH installed\r\n");
            }
        }

        // Some clients expect these anti-cheat DLLs to be present/loaded by a launcher/injector.
        // Our stubs (if present next to the client) keep the process moving further for protocol analysis.
        {
            HMODULE h1 = LoadLibraryA("TerSafe.dll");
            HMODULE h2 = LoadLibraryA("bdcap32.dll");
            HMODULE h3 = LoadLibraryA("start\\Tenprotect\\TerSafe.dll");
            if (h1 || h2 || h3) {
                g_stub_tersafe_base = (uintptr_t)h1;
                g_stub_tersafe_size = get_pe_image_size(h1);
                g_stub_bdcap_base = (uintptr_t)h2;
                g_stub_bdcap_size = get_pe_image_size(h2);
                g_stub_tersafe2_base = (uintptr_t)h3;
                g_stub_tersafe2_size = get_pe_image_size(h3);
                char buf[220];
                wsprintfA(buf, "[fmodex_proxy] preloaded anti-cheat stubs: TerSafe=%p bdcap32=%p start\\Tenprotect\\TerSafe=%p\r\n", h1, h2, h3);
                log_line(buf);
            }
        }

        // Install exit hooks as early as possible (the process may exit before our worker thread runs).
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "ExitProcess", (void *)&hook_ExitProcess,
                                   (void **)&g_real_ExitProcess);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "TerminateProcess", (void *)&hook_TerminateProcess,
                                   (void **)&g_real_TerminateProcess);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "LoadLibraryA", (void *)&hook_LoadLibraryA,
                                   (void **)&g_real_LoadLibraryA);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "LoadLibraryW", (void *)&hook_LoadLibraryW,
                                   (void **)&g_real_LoadLibraryW);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "GetProcAddress", (void *)&hook_GetProcAddress,
                                   (void **)&g_real_GetProcAddress);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "GetModuleHandleA", (void *)&hook_GetModuleHandleA,
                                   (void **)&g_real_GetModuleHandleA);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "GetModuleHandleW", (void *)&hook_GetModuleHandleW,
                                   (void **)&g_real_GetModuleHandleW);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "ntdll.dll", "RtlExitUserProcess", (void *)&hook_RtlExitUserProcess,
                                   (void **)&g_real_RtlExitUserProcess);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "ntdll.dll", "NtTerminateProcess", (void *)&hook_NtTerminateProcess,
                                   (void **)&g_real_NtTerminateProcess);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "ws2_32.dll", "connect", (void *)&hook_connect, (void **)&g_real_connect);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "CreateToolhelp32Snapshot", (void *)&hook_CreateToolhelp32Snapshot,
                                   (void **)&g_real_CreateToolhelp32Snapshot);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "Module32First", (void *)&hook_Module32First,
                                   (void **)&g_real_Module32First);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "Module32Next", (void *)&hook_Module32Next,
                                   (void **)&g_real_Module32Next);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "Module32FirstW", (void *)&hook_Module32FirstW,
                                   (void **)&g_real_Module32FirstW);
        patch_iat_import_in_module(GetModuleHandleW(NULL), "kernel32.dll", "Module32NextW", (void *)&hook_Module32NextW,
                                   (void **)&g_real_Module32NextW);
        HMODULE crt = GetModuleHandleA("msvcr71.dll");
        if (crt) {
            patch_iat_import_in_module(crt, "kernel32.dll", "ExitProcess", (void *)&hook_ExitProcess, (void **)&g_real_ExitProcess);
            patch_iat_import_in_module(crt, "kernel32.dll", "TerminateProcess", (void *)&hook_TerminateProcess,
                                       (void **)&g_real_TerminateProcess);
            patch_iat_import_in_module(crt, "ntdll.dll", "RtlExitUserProcess", (void *)&hook_RtlExitUserProcess,
                                       (void **)&g_real_RtlExitUserProcess);
            patch_iat_import_in_module(crt, "ntdll.dll", "NtTerminateProcess", (void *)&hook_NtTerminateProcess,
                                       (void **)&g_real_NtTerminateProcess);
        }

        HANDLE th = CreateThread(NULL, 0, install_hooks_thread, NULL, 0, NULL);
        if (th) {
            CloseHandle(th);
        }
    }
    if (reason == DLL_PROCESS_DETACH) {
        if (g_veh_handle) {
            RemoveVectoredExceptionHandler(g_veh_handle);
            g_veh_handle = NULL;
        }
        if (g_log != INVALID_HANDLE_VALUE) {
            CloseHandle(g_log);
            g_log = INVALID_HANDLE_VALUE;
        }
        if (g_real) {
            FreeLibrary(g_real);
            g_real = NULL;
        }
    }
    return TRUE;
}
