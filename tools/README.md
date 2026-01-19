# Hex-Rays C Cleanup Toolkit

Use `split_hexrays.py` to split the huge `df_game_r.c` into smaller compilable C files:

## Usage

```bash
python3 tools/split_hexrays.py df_game_r.c src 200
# generates:
# - include/prototypes.h
# - src/globals.c
# - src/part_001.c, src/part_002.c, ...
# - Makefile.multi
```

Then build with:

```bash
make -f Makefile.multi -j$(nproc)
```

Notes:
- The parser is heuristic. If it prints `Warning: no functions parsed`, tweak `FUNC_HEADER_RE` and `PROTO_RE`.
- Each `part_XXX.c` starts with standard includes and `prototypes.h`.
- If some missing prototypes/typedefs cause errors, add them to `defs.h` or edit `include/prototypes.h`.
- Link libraries are the same as `Makefile` (mysqlclient, GeoIP, pthread, rt, dl, m).
```
