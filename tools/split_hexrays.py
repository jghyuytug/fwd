#!/usr/bin/env python3
# split_hexrays.py - Split a huge Hex-Rays decompiled C file into smaller C units
# - Extracts top-level prototypes into include/prototypes.h
# - Writes globals (non-function top-level code) to src/globals.c
# - Splits functions into src/part_XXX.c (configurable per-file function count)
# Heuristics only; may need tweaks per project.

import os, re, sys, io
from typing import List

HEADER_INCLUDES = [
    '#include <math.h>\n',
    '#include <defs.h>\n',
    '#include <stdarg.h>\n',
    '#include "prototypes.h"\n',
]

FUNC_HEADER_RE = re.compile(r'^\s*[A-Za-z_][\w\s\*\(\),\[\]:<>]*\)\s*\{\s*$')
PROTO_RE       = re.compile(r'^\s*[A-Za-z_][\w\s\*\(\),\[\]:<>]*\)\s*;\s*$')

def usage():
    print('Usage: split_hexrays.py <input.c> [outdir] [per_file=200]')

class TopLevel:
    def __init__(self):
        self.globals: List[str] = []
        self.prototypes: List[str] = []
        self.funcs: List[List[str]] = []


def split_file(src_path: str, outdir: str, per_file: int = 200):
    os.makedirs(outdir, exist_ok=True)
    incldir = os.path.join(os.path.dirname(outdir), 'include')
    os.makedirs(incldir, exist_ok=True)

    with io.open(src_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    tl = TopLevel()

    i = 0
    n = len(lines)
    brace_depth = 0
    buf: List[str] = []
    in_func = False

    def flush_global(line: str):
        # drop the giant commented-out libc prototypes to reduce noise
        if line.lstrip().startswith('//') and '(' in line and ')' in line:
            return
        tl.globals.append(line)

    while i < n:
        line = lines[i]
        # detect function start when at toplevel and a header is followed by '{' on same line
        if brace_depth == 0 and not in_func:
            # join header over two lines if needed (header then '{' next line)
            if PROTO_RE.match(line):
                # top-level prototype: collect
                tl.prototypes.append(line.strip())
                i += 1
                continue
            # handle case header on one line and '{' next line
            if line.rstrip().endswith(')') and i + 1 < n and lines[i+1].lstrip().startswith('{'):
                # function header across two lines
                header = line
                body_first = lines[i+1]
                in_func = True
                brace_depth = 1
                buf = [header, body_first]
                i += 2
                continue
            if FUNC_HEADER_RE.match(line):
                in_func = True
                brace_depth = 1
                buf = [line]
                i += 1
                continue
            # otherwise treat as global content
            flush_global(line)
            i += 1
            continue
        # inside function body: track braces
        if in_func:
            if i < n:
                # count braces naively (does not account strings/comments)
                brace_depth += line.count('{')
                brace_depth -= line.count('}')
                buf.append(line)
                i += 1
                if brace_depth == 0:
                    tl.funcs.append(buf)
                    buf = []
                    in_func = False
            continue

    # write prototypes.h
    protos = sorted(set([p for p in tl.prototypes if not p.startswith('void (__cdecl *init_proc')]))
    with io.open(os.path.join(incldir, 'prototypes.h'), 'w', encoding='utf-8') as pf:
        pf.write('#ifndef PROTOTYPES_H\n#define PROTOTYPES_H\n\n')
        pf.write('// Auto-generated from Hex-Rays top-level prototypes\n')
        for inc in ['#include <stdint.h>\n', '#include <stddef.h>\n']:
            pf.write(inc)
        for p in protos:
            pf.write(p + '\n')
        pf.write('\n#endif\n')

    # write globals.c
    with io.open(os.path.join(outdir, 'globals.c'), 'w', encoding='utf-8') as gf:
        for inc in HEADER_INCLUDES:
            gf.write(inc)
        gf.write('\n')
        gf.writelines(tl.globals)

    # split functions into chunks
    if not tl.funcs:
        print('Warning: no functions parsed; check heuristics.')
    part_idx = 1
    func_idx = 0
    while func_idx < len(tl.funcs):
        chunk = tl.funcs[func_idx:func_idx+per_file]
        part_name = os.path.join(outdir, f'part_{part_idx:03d}.c')
        with io.open(part_name, 'w', encoding='utf-8') as cf:
            for inc in HEADER_INCLUDES:
                cf.write(inc)
            cf.write('\n')
            for fn in chunk:
                cf.writelines(fn)
                if not fn[-1].endswith('\n'):
                    cf.write('\n')
                cf.write('\n')
        part_idx += 1
        func_idx += per_file

    # write a helper Makefile (optional)
    mk = os.path.join(os.path.dirname(outdir), 'Makefile.multi')
    with io.open(mk, 'w', encoding='utf-8') as mf:
        mf.write('''# Auto-generated Makefile for multi-unit build\n''')
        mf.write('''CC=gcc\nCXX=g++\nTARGET=df_game_r\nCFLAGS=-m32 -O2 -g -fPIC -pipe\nCPPFLAGS=-I. -Iinclude -I/usr/include/mysql\nLDFLAGS=-m32 -Wl,-Map,build.map\nLIBS=-lpthread -lrt -ldl -lm -lmysqlclient -lGeoIP\nSRCS=$(wildcard src/part_*.c) src/globals.c\nOBJS=$(SRCS:.c=.o)\nall: $(TARGET)\n$(TARGET): $(OBJS)\n\t$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)\nclean:\n\trm -f $(OBJS) $(TARGET) build.map\n''')

    print(f'Wrote include/prototypes.h, src/globals.c and {part_idx-1} part files.')

if __name__ == '__main__':
    if len(sys.argv) < 2:
        usage(); sys.exit(1)
    src = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) >= 3 else os.path.join(os.path.dirname(src), 'src')
    per = int(sys.argv[3]) if len(sys.argv) >= 4 else 200
    split_file(src, out, per)
