# fmodex proxy (for running the dumped client)

This client imports several C++ FMOD Ex exports that are **not present** in the available `fmodex.dll`.

This tool generates a proxy `fmodex.dll` that:
- forwards all imported FMOD symbols to `fmodex_real.dll`
- implements a small set of missing exports

## Usage (WSL)

1) Prepare files (Windows):
- Put the client exe as `C:\wsl\client.exe`
- Put the original FMOD as `C:\wsl\fmodex_real.dll` (rename from `fmodex.dll`)

2) Build in WSL:
```bash
cd /mnt/c/Users/waw/Desktop/game/tools/fmodex_proxy
python3 gen_def.py /mnt/c/wsl/client.exe /mnt/c/wsl/fmodex_real.dll ./fmodex_proxy.def
i686-w64-mingw32-gcc -O2 -m32 -shared -o fmodex.dll fmodex_proxy.c fmodex_proxy.def -Wl,--enable-stdcall-fixup
```

3) Deploy (Windows):
- Copy `fmodex.dll` next to the client exe
- Keep `fmodex_real.dll` next to it as well

