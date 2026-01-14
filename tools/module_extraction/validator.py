#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Module Validator Tool for DNF Game Server
Purpose: Verify module compilation and dependencies
"""

import os
import re
import sys
from pathlib import Path
from typing import List, Dict, Set, Tuple
from collections import defaultdict

class ModuleValidator:
    """Validate module structure, includes, and dependencies"""

    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.undefined_symbols: Dict[str, List[str]] = defaultdict(list)
        self.include_paths: Dict[str, Set[str]] = defaultdict(set)
        self.module_dependencies: Dict[str, Set[str]] = defaultdict(set)

    def check_includes(self, module: str) -> Tuple[List[str], List[str]]:
        """Verify all #include statements are valid"""
        print(f"[1/4] Checking includes in {module} module...")

        module_dir = self.project_root / "src" / module.lower()
        valid_includes = []
        invalid_includes = []

        if not module_dir.exists():
            print(f"  ERROR: Module directory not found: {module_dir}")
            return valid_includes, invalid_includes

        # Scan all source files
        for src_file in module_dir.rglob("*.c") + module_dir.rglob("*.cpp"):
            print(f"  Checking {src_file.name}...", end=" ", flush=True)

            with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Extract all #include statements
            includes = re.findall(r'#include\s*[<"]([^>"]+)[>"]', content)
            found_invalid = 0

            for inc_path in includes:
                # Resolve include path
                if inc_path.startswith("../"):
                    # Relative path from source file
                    resolved = (src_file.parent / inc_path).resolve()
                else:
                    # Look in include/ directory
                    resolved = self.project_root / "include" / inc_path

                if resolved.exists():
                    valid_includes.append(inc_path)
                    self.include_paths[module].add(inc_path)
                else:
                    invalid_includes.append(inc_path)
                    found_invalid += 1

            if found_invalid > 0:
                print(f"found {found_invalid} invalid")
            else:
                print("OK")

        print(f"  Valid: {len(valid_includes)}, Invalid: {len(invalid_includes)}")
        return valid_includes, invalid_includes

    def find_undefined_symbols(self, module: str) -> Dict[str, List[str]]:
        """Find undefined symbols in compiled module"""
        print(f"[2/4] Finding undefined symbols in {module} module...")

        module_dir = self.project_root / "src" / module.lower()
        undefined = defaultdict(list)

        if not module_dir.exists():
            print(f"  ERROR: Module directory not found: {module_dir}")
            return undefined

        # Scan for external function calls
        for src_file in module_dir.rglob("*.c") + module_dir.rglob("*.cpp"):
            print(f"  Analyzing {src_file.name}...", end=" ", flush=True)

            with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Find function calls (simplified pattern)
            # Pattern: word( that's not preceded by 'void', 'int', etc (likely function def)
            calls = re.findall(r'(?<![a-zA-Z_])([a-zA-Z_]\w*)\s*\(', content)

            extern_symbols = set()
            for call in calls:
                # Check if it's likely external (not defined in same file)
                if f"{call}(" not in content or content.count(f"{call}(") > 5:
                    extern_symbols.add(call)

            print(f"found {len(extern_symbols)} external symbols")

            # Verify each external symbol exists in declaration file
            decl_file = self.project_root / "src_split" / "df_game_r_decl.h"
            if decl_file.exists():
                with open(decl_file, 'r', encoding='utf-8', errors='ignore') as df:
                    decl_content = df.read()

                for sym in extern_symbols:
                    if not re.search(rf'\b{sym}\b', decl_content):
                        undefined[src_file.name].append(sym)

        self.undefined_symbols[module] = undefined
        print(f"  Total undefined: {sum(len(v) for v in undefined.values())}")
        return undefined

    def analyze_dependencies(self, module: str) -> Dict[str, Set[str]]:
        """Analyze dependencies between modules"""
        print(f"[3/4] Analyzing dependencies for {module} module...")

        module_dir = self.project_root / "src" / module.lower()
        dependencies = set()

        if not module_dir.exists():
            print(f"  ERROR: Module directory not found: {module_dir}")
            return dependencies

        # Check include files for cross-module includes
        for src_file in module_dir.rglob("*.c") + module_dir.rglob("*.cpp"):
            with open(src_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            includes = re.findall(r'#include\s*[<"]([^>"]+)[>"]', content)

            for inc in includes:
                # Check if it's another module's header
                if "include/" in inc:
                    for potential_module in ["database", "network", "user", "inventory",
                                           "events", "security", "game", "common"]:
                        if potential_module in inc and potential_module != module.lower():
                            dependencies.add(potential_module.capitalize())

        self.module_dependencies[module] = dependencies

        if dependencies:
            print(f"  Dependencies: {', '.join(sorted(dependencies))}")
        else:
            print(f"  No external module dependencies found")

        return dependencies

    def generate_report(self, output_file: str = None) -> str:
        """Generate comprehensive validation report"""
        print(f"[4/4] Generating validation report...")

        report = []
        report.append("=" * 70)
        report.append("Module Validation Report")
        report.append("Generated: 2025-11-05")
        report.append("=" * 70)
        report.append("")

        # Summary
        report.append("VALIDATION SUMMARY")
        report.append("-" * 70)

        modules_checked = len(self.include_paths)
        total_includes = sum(len(v) for v in self.include_paths.values())
        total_undefined = sum(len(v) for v in self.undefined_symbols.values())

        report.append(f"Modules checked: {modules_checked}")
        report.append(f"Total include statements: {total_includes}")
        report.append(f"Total undefined symbols: {total_undefined}")
        report.append("")

        # Include details
        if self.include_paths:
            report.append("INCLUDE STATEMENTS")
            report.append("-" * 70)
            for module, includes in sorted(self.include_paths.items()):
                report.append(f"\n{module} module ({len(includes)} includes):")
                for inc in sorted(includes)[:5]:  # Show first 5
                    report.append(f"  - {inc}")
                if len(includes) > 5:
                    report.append(f"  ... and {len(includes) - 5} more")
            report.append("")

        # Undefined symbols
        if self.undefined_symbols:
            report.append("UNDEFINED SYMBOLS")
            report.append("-" * 70)
            for module, undefined_dict in sorted(self.undefined_symbols.items()):
                if undefined_dict:
                    report.append(f"\n{module} module:")
                    for src_file, symbols in sorted(undefined_dict.items()):
                        if symbols:
                            report.append(f"  {src_file}: {len(symbols)} undefined")
                            for sym in list(symbols)[:3]:  # Show first 3
                                report.append(f"    - {sym}")
            report.append("")

        # Dependencies
        if self.module_dependencies:
            report.append("MODULE DEPENDENCIES")
            report.append("-" * 70)
            for module, deps in sorted(self.module_dependencies.items()):
                if deps:
                    report.append(f"{module}: depends on {', '.join(sorted(deps))}")
                else:
                    report.append(f"{module}: no external dependencies")
            report.append("")

        # Recommendations
        report.append("RECOMMENDATIONS")
        report.append("-" * 70)
        report.append("1. Fix invalid includes before compilation")
        report.append("2. Resolve undefined symbols or create adapters")
        report.append("3. Consider dependency injection for circular dependencies")
        report.append("4. Create module test files to validate compilation")
        report.append("")

        report.append("=" * 70)

        report_text = "\n".join(report)

        if output_file:
            with open(output_file, 'w') as f:
                f.write(report_text)
            print(f"  Report saved to: {output_file}")

        return report_text


def main():
    """Main entry point"""
    project_root = Path(__file__).parent.parent.parent

    print("=" * 70)
    print("Module Validator Tool - DNF Game Server")
    print("=" * 70)
    print()

    validator = ModuleValidator(str(project_root))

    # Validate each module
    for module in ["Security", "Common", "Database", "Network"]:
        print(f"\nValidating {module} module...")
        print("-" * 70)

        valid_incs, invalid_incs = validator.check_includes(module)
        validator.find_undefined_symbols(module)
        validator.analyze_dependencies(module)

        print()

    # Generate report
    report_path = project_root / "docs" / "generated" / "MODULE_VALIDATION_REPORT.md"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    validator.generate_report(str(report_path))

    print()
    print("=" * 70)
    print("Validation complete!")
    print("=" * 70)


if __name__ == "__main__":
    main()
