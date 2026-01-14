#!/usr/bin/env python3
"""
MySQL Parameter Name Fixer
Fixes decompiler artifacts where parameter names don't match usage.
Specifically fixes 'a2' references that should be 'id' or other declared parameters.
"""

import re
import sys

def fix_mysql_cpp(file_path):
    """Fix parameter names in MySQL.cpp"""

    with open(file_path, 'r') as f:
        content = f.read()

    original_content = content

    # Pattern: function with 'int id' parameter but using 'a2'
    # Fix: Replace a2 with id in functions that have 'int id' parameter

    replacements = [
        # Pattern 1: int id, ... functions using a2 - replace a2 with id
        (r'(\(MySQL \*this, int id,.*?\n{.*?)a2(?![\w])',
         r'\1id'),

        # Pattern 2: specific fix for get_int first parameter issue
        (r'int __cdecl MySQL::get_int\(MySQL \*this, int id, unsigned int \*a3\)\n{\n  return MySQL::get_uint\(this, a2,',
         'int __cdecl MySQL::get_int(MySQL *this, int id, unsigned int *a3)\n{\n  return MySQL::get_uint(this, id,'),
    ]

    # More comprehensive approach: parse function by function
    lines = content.split('\n')
    fixed_lines = []
    i = 0

    while i < len(lines):
        line = lines[i]

        # Check if this is a function declaration line
        if '__cdecl MySQL::' in line and '(' in line:
            # Extract function signature
            func_sig = line
            i += 1

            # Get parameter names from signature
            params = extract_params(func_sig)

            # Now process function body
            fixed_lines.append(func_sig)

            while i < len(lines):
                body_line = lines[i]

                # Check for function end
                if body_line.startswith('}') and (i == 0 or not lines[i-1].strip().startswith('*')):
                    fixed_lines.append(body_line)
                    i += 1
                    break

                # Fix parameter references in body
                fixed_body_line = fix_parameter_refs(body_line, params, func_sig)
                fixed_lines.append(fixed_body_line)
                i += 1
        else:
            fixed_lines.append(line)
            i += 1

    fixed_content = '\n'.join(fixed_lines)

    # Print stats
    if fixed_content != original_content:
        print(f"✅ Fixed parameter names in {file_path}")

        # Count changes
        changes = sum(1 for a, b in zip(original_content.split('\n'), fixed_content.split('\n')) if a != b)
        print(f"   Lines modified: {changes}")

        # Write back
        with open(file_path, 'w') as f:
            f.write(fixed_content)

        return True
    else:
        print(f"ℹ️  No changes needed in {file_path}")
        return False

def extract_params(func_sig):
    """Extract parameter names from function signature"""
    # Match: type name, type name, ...
    params = {}

    # Find opening paren
    paren_pos = func_sig.find('(')
    if paren_pos == -1:
        return params

    # Find closing paren
    close_paren = func_sig.find(')')
    if close_paren == -1:
        return params

    param_str = func_sig[paren_pos+1:close_paren]

    # Split by comma
    for param in param_str.split(','):
        param = param.strip()
        if not param or param == 'void':
            continue

        # Last word is parameter name
        parts = param.split()
        if len(parts) >= 2:
            param_name = parts[-1].replace('*', '').strip()
            if param_name:
                params[param_name] = True

    return params

def fix_parameter_refs(line, params, func_sig):
    """Fix parameter references in function body"""

    # If 'int id' is a parameter, fix references to undefined a2
    if 'id' in params and 'a2' not in params:
        # Replace a2 with id only if a2 is not defined
        line = re.sub(r'\ba2\b(?![\w])', 'id', line)

    # If 'a4' is used but not defined, but 'name' is parameter
    if 'name' in params and 'a4' not in params:
        # In some functions, a4 should be name
        if 'int __cdecl MySQL::open' in func_sig:
            line = re.sub(r'\ba4\b', 'name', line)

    # If 'a5' is used but not defined, but 'src' is parameter
    if 'src' in params and 'a5' not in params:
        line = re.sub(r'\ba5\b', 'src', line)

    # If 'a6' is used but not defined
    if 'a6' not in params and '__cdecl MySQL::open' in func_sig:
        # In open functions, a6 might not have a clear mapping
        pass

    return line

def main():
    mysql_cpp = 'src/database/MySQL.cpp'

    print("🔧 MySQL Parameter Name Fixer")
    print("=" * 50)
    print()

    try:
        fix_mysql_cpp(mysql_cpp)
        print()
        print("✅ Parameter fixing complete!")
        return 0
    except Exception as e:
        print(f"❌ Error: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())
