#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Module Extractor Tool for DNF Game Server Modularization
Purpose: Automatically extract and organize code by module
"""

import os
import re
import sys
from pathlib import Path
from typing import List, Dict, Set, Tuple

class ModuleExtractor:
    """Extract and classify functions/code by module"""

    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.decl_file = self.project_root / "src_split" / "df_game_r_decl.h"
        self.src_split_dir = self.project_root / "src_split"
        self.functions_by_module: Dict[str, List[str]] = {}

    def parse_declarations(self) -> Dict[str, List[str]]:
        """Parse df_game_r_decl.h and extract function declarations by module"""
        print("[1/5] Parsing function declarations...")

        functions = {}
        if not self.decl_file.exists():
            print(f"ERROR: Declaration file not found: {self.decl_file}")
            return functions

        with open(self.decl_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        # Parse function declarations
        # Patterns:
        # type __cdecl func_name(args);
        # type func_name();
        # ptr = __cdecl func_name(...);

        pattern = r'(?:__cdecl\s+)?(\w+)\s*\([^)]*\)(?:\s*;|\s*__cdecl)?'

        total = 0
        for line in lines:
            # Skip comments
            if line.strip().startswith("//"):
                continue

            # Look for function-like declarations
            matches = re.finditer(r'(?:__cdecl\s+)*(\w+)\s*\(', line)
            for match in matches:
                func_name = match.group(1)

                # Skip common false positives
                if func_name in ["include", "define", "ifdef", "if", "for", "while",
                               "switch", "catch", "struct", "class", "template", "namespace"]:
                    continue

                total += 1

                # Classify by prefix
                module = self._classify_function(func_name)
                if module not in functions:
                    functions[module] = []
                functions[module].append(func_name)

        print(f"  Found {total} function declarations")
        print(f"  Classified into {len(functions)} modules")
        for module, funcs in sorted(functions.items()):
            print(f"    {module}: {len(funcs)} functions")

        return functions

    def _classify_function(self, func_name: str) -> str:
        """Classify function into module by prefix"""
        # Module prefix mapping (order matters - try longer prefixes first)
        prefixes = {
            "Security": ["CHackAnalyzer", "CAutoPunishRule", "CGMAccounts", "WongWork"],
            "Database": ["CDBConnector", "MySQL", "DBMgr", "CQueryCounter", "ARAD::DATABASE"],
            "Network": ["CNetwork", "CSession", "PacketBuf", "CEpoll", "exchange_server"],
            "User": ["CUser", "CUserCharac", "CGM", "Login"],
            "Inventory": ["CInventory", "Inven_Item", "Warehouse"],
            "Events": ["CEvent", "EventClassify", "Bingo", "Attendance"],
            "Game": ["CDungeon", "CParty", "CSkill", "QuickParty", "CBattle"],
            "Common": ["Common", "Constants", "Error", "Util"],
        }

        for module, module_prefixes in prefixes.items():
            for prefix in module_prefixes:
                if prefix.lower() in func_name.lower():
                    return module

        return "UNKNOWN"

    def classify_by_prefix(self) -> Dict[str, List[str]]:
        """Classify all functions by module prefix"""
        print("[2/5] Classifying functions by prefix...")
        self.functions_by_module = self.parse_declarations()
        return self.functions_by_module

    def extract_source_segments(self, module: str) -> List[Tuple[str, str]]:
        """Extract source code segments for a specific module from src_split/"""
        print(f"[3/5] Extracting source segments for {module} module...")

        if module not in self.functions_by_module:
            print(f"  Module {module} not found")
            return []

        functions = self.functions_by_module[module]
        segments = []

        # Search through all split files
        for part_file in sorted(self.src_split_dir.glob("df_game_r_part*.c")):
            print(f"  Searching in {part_file.name}...", end=" ", flush=True)

            with open(part_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            found_count = 0
            for func_name in functions:
                # Find function implementation (simplified pattern)
                pattern = rf'{func_name}\s*\([^)]*\)\s*{{'
                if re.search(pattern, content):
                    segments.append((func_name, part_file.name))
                    found_count += 1

            if found_count > 0:
                print(f"found {found_count}")
            else:
                print("none")

        print(f"  Total segments found: {len(segments)}")
        return segments

    def fix_includes(self, source_file: str, module: str) -> str:
        """Fix #include paths in source file for module"""
        print(f"[4/5] Fixing includes in {source_file}...")

        with open(source_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Fix relative includes
        patterns = {
            r'#include\s*"\.\.\/\.\.\/include\/': f'#include "../../../include/',
            r'#include\s*"include\/': f'#include "../../include/',
            r'#include\s*"src_split\/': f'#include "../../src_split/',
        }

        for pattern, replacement in patterns.items():
            content = re.sub(pattern, replacement, content)

        # Write back
        with open(source_file, 'w', encoding='utf-8') as f:
            f.write(content)

        return content

    def generate_makefile(self, module: str, output_dir: str) -> str:
        """Generate CMakeLists.txt or Makefile for a module"""
        print(f"[5/5] Generating Makefile for {module} module...")

        makefile_content = f"""# Auto-generated Makefile for {module} module
# Module: {module}
# Generated: 2025-11-05

CC = gcc
CFLAGS = -m32 -O2 -g -fPIC -pipe -I../../include
LDFLAGS = -m32
LIBS = -lpthread -lrt -ldl -lm

SRCROOT = ../..
INCROOT = ${{SRCROOT}}/include
BUILDROOT = ${{SRCROOT}}/build
LIBROOT = ${{SRCROOT}}/lib

# Source files for {module} module
{module.upper()}_SOURCES = \\
    $(wildcard *.cpp) \\
    $(wildcard *.c)

{module.upper()}_HEADERS = \\
    ${{INCROOT}}/{module.lower()}/headers/*.h

{module.upper()}_OBJECTS = \\
    $(patsubst %.cpp,${{BUILDROOT}}/{module.lower()}/%.o,$(filter %.cpp,$({module.upper()}_SOURCES))) \\
    $(patsubst %.c,${{BUILDROOT}}/{module.lower()}/%.o,$(filter %.c,$({module.upper()}_SOURCES)))

.PHONY: all clean

all: ${{LIBROOT}}/lib{module.lower()}.a

${{LIBROOT}}/lib{module.lower()}.a: $(${module.upper()}_OBJECTS)
	@mkdir -p ${{LIBROOT}}
	@ar rcs $@ $^
	@echo "Built {module} library: $@"

${{BUILDROOT}}/{module.lower()}:
	@mkdir -p $@

${{BUILDROOT}}/{module.lower()}/%.o: %.c | ${{BUILDROOT}}/{module.lower()}
	$(CC) $(CFLAGS) -c $< -o $@

${{BUILDROOT}}/{module.lower()}/%.o: %.cpp | ${{BUILDROOT}}/{module.lower()}
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf ${{BUILDROOT}}/{module.lower()}/*.o
	@rm -f ${{LIBROOT}}/lib{module.lower()}.a
	@echo "Cleaned {module} module"

verify:
	@echo "Verifying {module} module..."
	@test -f ${{LIBROOT}}/lib{module.lower()}.a && echo "OK: lib{module.lower()}.a exists" || echo "ERROR: lib{module.lower()}.a not found"

"""

        output_path = Path(output_dir) / "Makefile"
        with open(output_path, 'w') as f:
            f.write(makefile_content)

        print(f"  Generated: {output_path}")
        return makefile_content


def main():
    """Main entry point"""
    project_root = Path(__file__).parent.parent.parent

    print("=" * 60)
    print("Module Extractor Tool - DNF Game Server")
    print("=" * 60)
    print()

    extractor = ModuleExtractor(str(project_root))

    # Step 1: Parse declarations
    functions = extractor.classify_by_prefix()
    print()

    # Step 2: Extract segments for Security module (first priority)
    print("Extracting Security module (highest priority)...")
    security_segments = extractor.extract_source_segments("Security")
    print()

    # Step 3: Generate Makefile for Security
    security_dir = project_root / "src" / "security"
    extractor.generate_makefile("Security", str(security_dir))
    print()

    print("=" * 60)
    print("Module extraction complete!")
    print("Next steps:")
    print("  1. Review extracted functions in each module")
    print("  2. Copy source files to src/<module>/")
    print("  3. Fix #include paths")
    print("  4. Compile and resolve dependencies")
    print("=" * 60)


if __name__ == "__main__":
    main()
