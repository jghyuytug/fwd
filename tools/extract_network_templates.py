#!/usr/bin/env python3
"""
Extract CNetwork, CEpoll, and CSessionManager template class implementations
from split source files.
"""

import re
import sys
from pathlib import Path

class TemplateExtractor:
    def __init__(self, src_dir="src_split"):
        self.src_dir = Path(src_dir)
        self.output_dir = Path("src/network")
        self.include_dir = Path("include/network")

        # Function locations discovered by grep
        self.cnetwork_4096_locations = [
            ("df_game_r_part08.c", 18589),
            ("df_game_r_part09.c", 44153),
            ("df_game_r_part09.c", 44213),
            ("df_game_r_part09.c", 44806),
            ("df_game_r_part09.c", 44820),
            ("df_game_r_part10.c", 6224),
            ("df_game_r_part10.c", 31658),
            ("df_game_r_part10.c", 84181),
            ("df_game_r_part10.c", 84187),
            ("df_game_r_part10.c", 84193),
            ("df_game_r_part10.c", 84199),
            ("df_game_r_part15.c", 14196),
            ("df_game_r_part15.c", 14267),
            ("df_game_r_part15.c", 14420),
            ("df_game_r_part15.c", 14426),
            ("df_game_r_part16.c", 85835),
            ("df_game_r_part16.c", 85883),
            ("df_game_r_part16.c", 85949),
            ("df_game_r_part16.c", 85959),
            ("df_game_r_part16.c", 86076),
            ("df_game_r_part16.c", 86096),
            ("df_game_r_part16.c", 87174),
        ]

        self.cnetwork_6144_locations = [
            ("df_game_r_part08.c", 49968),
            ("df_game_r_part08.c", 49987),
            ("df_game_r_part08.c", 50018),
            ("df_game_r_part08.c", 50033),
            ("df_game_r_part08.c", 50081),
            ("df_game_r_part08.c", 50154),
            ("df_game_r_part08.c", 50230),
            ("df_game_r_part08.c", 50236),
            ("df_game_r_part08.c", 50246),
            ("df_game_r_part08.c", 50252),
        ]

        self.cepoll_locations = [
            ("df_game_r_part13.c", 64418),
            ("df_game_r_part13.c", 65415),
            ("df_game_r_part13.c", 65928),
            ("df_game_r_part13.c", 66196),
            ("df_game_r_part13.c", 66747),
        ]

        self.csessionmgr_locations = [
            ("df_game_r_part13.c", 64425),
            ("df_game_r_part13.c", 64433),
            ("df_game_r_part13.c", 65237),
            ("df_game_r_part13.c", 65295),
            ("df_game_r_part13.c", 65385),
            ("df_game_r_part13.c", 65402),
            ("df_game_r_part13.c", 66044),
        ]

    def extract_function(self, file_path, line_num):
        """Extract a single function starting from line_num"""
        lines = []
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            all_lines = f.readlines()

        # Start from the given line (0-indexed, but line_num is 1-indexed)
        idx = line_num - 1
        if idx >= len(all_lines):
            return None

        # Read function signature
        func_line = all_lines[idx].rstrip()
        lines.append(func_line)
        idx += 1

        # Count braces to find function end
        brace_count = 0
        in_function = False

        while idx < len(all_lines):
            line = all_lines[idx].rstrip()
            lines.append(line)

            # Count braces
            for char in line:
                if char == '{':
                    brace_count += 1
                    in_function = True
                elif char == '}':
                    brace_count -= 1

            # Function ends when braces balance
            if in_function and brace_count == 0:
                break

            idx += 1

        return '\n'.join(lines)

    def extract_all_functions(self, locations, class_name):
        """Extract all functions for a given class"""
        functions = []

        for file_name, line_num in locations:
            file_path = self.src_dir / file_name
            print(f"  [{class_name}] Extracting from {file_name}:{line_num}")

            func_code = self.extract_function(file_path, line_num)
            if func_code:
                functions.append(func_code)
            else:
                print(f"    WARNING: Could not extract function at {file_name}:{line_num}")

        return functions

    def create_cnetwork_file(self):
        """Create CNetwork.cpp with both template instantiations"""
        print("\n[*] Extracting CNetwork template class...")

        # Extract CNetwork<4096,450000> functions
        print("  Extracting CNetwork<4096,450000> functions...")
        funcs_4096 = self.extract_all_functions(self.cnetwork_4096_locations, "CNetwork<4096,450000>")

        # Extract CNetwork<6144,100000> functions
        print("  Extracting CNetwork<6144,100000> functions...")
        funcs_6144 = self.extract_all_functions(self.cnetwork_6144_locations, "CNetwork<6144,100000>")

        # Create output file
        output = []
        output.append("// CNetwork Template Class Implementations")
        output.append("// Extracted from decompiled source")
        output.append("// Template instantiations: <4096,450000> and <6144,100000>")
        output.append("")
        output.append("#include \"network/CNetwork.h\"")
        output.append("#include <sys/socket.h>")
        output.append("#include <netinet/in.h>")
        output.append("#include <arpa/inet.h>")
        output.append("#include <cstring>")
        output.append("")
        output.append("// =================================================================")
        output.append("// CNetwork<4096,450000> Implementation (Game Server)")
        output.append("// Buffer: 4096 bytes, Max connections: 450000")
        output.append("// =================================================================")
        output.append("")

        for func in funcs_4096:
            output.append(func)
            output.append("")

        output.append("// =================================================================")
        output.append("// CNetwork<6144,100000> Implementation (Alternative Configuration)")
        output.append("// Buffer: 6144 bytes, Max connections: 100000")
        output.append("// =================================================================")
        output.append("")

        for func in funcs_6144:
            output.append(func)
            output.append("")

        # Write output
        output_path = self.output_dir / "CNetwork.cpp"
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output))

        print(f"  Created: {output_path}")
        print(f"  Functions: {len(funcs_4096)} + {len(funcs_6144)} = {len(funcs_4096) + len(funcs_6144)}")

        return len(funcs_4096) + len(funcs_6144)

    def create_cepoll_file(self):
        """Create CEpoll.cpp"""
        print("\n[*] Extracting CEpoll template class...")

        funcs = self.extract_all_functions(self.cepoll_locations, "CEpoll<CSession>")

        # Create output file
        output = []
        output.append("// CEpoll Template Class Implementation")
        output.append("// Extracted from decompiled source")
        output.append("// Template instantiation: CEpoll<exchange_server::CSession>")
        output.append("")
        output.append("#include \"network/CEpoll.h\"")
        output.append("#include \"network/CSession.h\"")
        output.append("#include <sys/epoll.h>")
        output.append("#include <unistd.h>")
        output.append("")
        output.append("namespace exchange_server {")
        output.append("")

        for func in funcs:
            output.append(func)
            output.append("")

        output.append("} // namespace exchange_server")

        # Write output
        output_path = self.output_dir / "CEpoll.cpp"
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output))

        print(f"  Created: {output_path}")
        print(f"  Functions: {len(funcs)}")

        return len(funcs)

    def create_csessionmanager_file(self):
        """Create CSessionManager.cpp"""
        print("\n[*] Extracting CSessionManager template class...")

        funcs = self.extract_all_functions(self.csessionmgr_locations, "CSessionManager")

        # Create output file
        output = []
        output.append("// CSessionManager Template Class Implementation")
        output.append("// Extracted from decompiled source")
        output.append("// Template instantiation: CSessionManager<CSession, PacketGuard, CEpoll>")
        output.append("")
        output.append("#include \"network/CSessionManager.h\"")
        output.append("#include \"network/CSession.h\"")
        output.append("#include \"network/CEpoll.h\"")
        output.append("#include <sys/epoll.h>")
        output.append("")
        output.append("namespace exchange_server {")
        output.append("")

        for func in funcs:
            output.append(func)
            output.append("")

        output.append("} // namespace exchange_server")

        # Write output
        output_path = self.output_dir / "CSessionManager.cpp"
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output))

        print(f"  Created: {output_path}")
        print(f"  Functions: {len(funcs)}")

        return len(funcs)

    def run(self):
        """Main extraction process"""
        print("=" * 70)
        print("Network Template Class Extractor")
        print("=" * 70)

        # Create output directories if needed
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)

        # Extract each template class
        cnetwork_count = self.create_cnetwork_file()
        cepoll_count = self.create_cepoll_file()
        csessionmgr_count = self.create_csessionmanager_file()

        # Summary
        total = cnetwork_count + cepoll_count + csessionmgr_count
        print("\n" + "=" * 70)
        print("Extraction Complete!")
        print("=" * 70)
        print(f"  CNetwork functions:        {cnetwork_count}")
        print(f"  CEpoll functions:          {cepoll_count}")
        print(f"  CSessionManager functions: {csessionmgr_count}")
        print(f"  Total functions extracted: {total}")
        print("=" * 70)
        print("")
        print("Next steps:")
        print("  1. Create header files (CNetwork.h, CEpoll.h, CSessionManager.h)")
        print("  2. Update Makefile.network to include new source files")
        print("  3. Test compilation: make -f Makefile.network all")
        print("")

if __name__ == "__main__":
    extractor = TemplateExtractor()
    extractor.run()
