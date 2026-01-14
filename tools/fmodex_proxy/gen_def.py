import sys
from pathlib import Path

import pefile


MISSING_EXPORT_MAP = {
    "?getDriverInfo@System@FMOD@@QAG?AW4FMOD_RESULT@@HPADHPAUFMOD_GUID@@@Z": "proxy_getDriverInfo",
    "?getOpenState@Sound@FMOD@@QAG?AW4FMOD_RESULT@@PAW4FMOD_OPENSTATE@@PAIPA_N2@Z": "proxy_getOpenState",
    "?getDriverCaps@System@FMOD@@QAG?AW4FMOD_RESULT@@HPAIPAHPAW4FMOD_SPEAKERMODE@@@Z": "proxy_getDriverCaps",
}

# Some symbols exist in the "real" FMOD DLL, but we still want to override behavior.
FORCE_PROXY_MAP = {
    "?getVersion@System@FMOD@@QAG?AW4FMOD_RESULT@@PAI@Z": "proxy_getVersion",
}


def load_export_names(dll_path: Path) -> set[str]:
    pe = pefile.PE(str(dll_path), fast_load=True)
    pe.parse_data_directories(
        directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]]
    )
    names: set[str] = set()
    if hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        for sym in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            if sym.name:
                names.add(sym.name.decode("ascii", "ignore"))
    return names


def load_fmod_imports(exe_path: Path) -> list[str]:
    pe = pefile.PE(str(exe_path))
    imports: list[str] = []
    for entry in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        dll = entry.dll.decode("ascii", "ignore").lower()
        if dll != "fmodex.dll":
            continue
        for imp in entry.imports:
            if imp.name is None:
                raise RuntimeError("ordinal import not supported for fmodex.dll")
            imports.append(imp.name.decode("ascii", "ignore"))
    if not imports:
        raise RuntimeError("no fmodex.dll imports found")
    return imports


def main() -> int:
    if len(sys.argv) != 4:
        print("Usage: gen_def.py <client.exe> <fmodex_real.dll> <out.def>")
        return 2

    exe_path = Path(sys.argv[1]).resolve()
    real_dll_path = Path(sys.argv[2]).resolve()
    out_path = Path(sys.argv[3]).resolve()

    exports = load_export_names(real_dll_path)
    imports = load_fmod_imports(exe_path)

    lines: list[str] = []
    lines.append("LIBRARY fmodex")
    lines.append("EXPORTS")

    missing: list[str] = []
    for name in imports:
        internal = FORCE_PROXY_MAP.get(name)
        if internal:
            lines.append(f'  "{name}"={internal}')
            continue

        if name in exports:
            lines.append(f'  "{name}"=fmodex_real."{name}"')
            continue

        internal = MISSING_EXPORT_MAP.get(name)
        if not internal:
            missing.append(name)
            continue
        lines.append(f'  "{name}"={internal}')

    if missing:
        print("Missing exports not covered by MISSING_EXPORT_MAP:")
        for m in missing:
            print("  ", m)
        return 1

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"Wrote: {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
