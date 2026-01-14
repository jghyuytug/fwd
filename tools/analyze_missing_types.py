#!/usr/bin/env python3
"""
Analyze compilation log to identify and classify missing types
"""

import re
from collections import Counter
from pathlib import Path

def extract_missing_types(log_file):
    """Extract all 'was not declared' and 'incomplete type' errors"""
    pattern = re.compile(r"error: '([^']+)' was not declared|error: aggregate '([^']+)' has incomplete type")

    types = []
    with open(log_file, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            matches = pattern.findall(line)
            for match in matches:
                type_name = match[0] or match[1]
                if type_name:
                    types.append(type_name)

    return types

def classify_type(type_name):
    """Classify type as enum, namespace, or class/struct"""
    # Remove template parameters and scope resolution
    base_name = re.sub(r'<.*>', '', type_name)

    # Check if it's a namespace (followed by ::)
    if '::' in type_name and not base_name.endswith('::'):
        parts = type_name.split('::')
        if len(parts) > 1:
            return 'namespace', parts[0]

    # Check if it's an enum
    if 'ENUM' in type_name.upper():
        return 'enum', None

    # Default to class/struct
    return 'class/struct', None

def infer_module(type_name):
    """Infer module based on type name patterns"""
    name_lower = type_name.lower()

    # Module patterns
    patterns = {
        'network': ['packet', 'session', 'network', 'epoll', 'socket'],
        'database': ['db', 'mysql', 'query', 'database'],
        'cash_shop': ['cash', 'nexon', 'taiwan'],
        'pvp': ['pvp', 'assault', 'arena'],
        'dungeon': ['dungeon', 'battle', 'field', 'monster'],
        'party': ['party', 'guild', 'team'],
        'inventory': ['inven', 'inventory', 'item'],
        'character': ['character', 'charac', 'user'],
        'event': ['event', 'quest'],
        'skill': ['skill', 'buff', 'debuff'],
        'security': ['hack', 'anti', 'cheat', 'gm'],
        'altar': ['altar', 'advancealtar'],
        'auction': ['auction', 'trade', 'exchange'],
        'achievement': ['achievement', 'title'],
        'core': ['common', 'util', 'base', 'string', 'vector', 'map'],
    }

    for module, keywords in patterns.items():
        for keyword in keywords:
            if keyword in name_lower:
                return module

    return 'unknown'

def generate_report(log_file, output_file):
    """Generate classification report"""
    print(f"Analyzing {log_file}...")

    # Extract types
    types = extract_missing_types(log_file)
    print(f"Found {len(types)} type reference errors")

    # Count frequencies
    type_counts = Counter(types)
    unique_types = len(type_counts)
    print(f"Unique types: {unique_types}")

    # Classify types
    classifications = {}
    for type_name, count in type_counts.items():
        type_class, namespace = classify_type(type_name)
        module = infer_module(type_name)
        classifications[type_name] = {
            'count': count,
            'classification': type_class,
            'namespace': namespace,
            'module': module
        }

    # Count by classification
    enum_count = sum(1 for c in classifications.values() if c['classification'] == 'enum')
    namespace_count = sum(1 for c in classifications.values() if c['classification'] == 'namespace')
    class_count = sum(1 for c in classifications.values() if c['classification'] == 'class/struct')

    # Sort by count
    sorted_types = sorted(classifications.items(), key=lambda x: x[1]['count'], reverse=True)

    # Generate markdown report
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("# Phase 1: Core Type Classification Report\n\n")
        f.write("## Executive Summary\n\n")
        f.write(f"- **Total type reference errors**: {len(types):,}\n")
        f.write(f"- **Unique missing types**: {unique_types:,}\n")
        f.write(f"- **Enumerations**: {enum_count} ({enum_count*100//unique_types}%)\n")
        f.write(f"- **Namespaces**: {namespace_count} ({namespace_count*100//unique_types}%)\n")
        f.write(f"- **Classes/Structs**: {class_count} ({class_count*100//unique_types}%)\n\n")

        # Top 50 types
        f.write("## Top 50 Missing Types\n\n")
        f.write("| Rank | Type Name | Count | Classification | Module |\n")
        f.write("|------|-----------|-------|----------------|--------|\n")

        for i, (type_name, info) in enumerate(sorted_types[:50], 1):
            namespace_info = f" ({info['namespace']}::)" if info['namespace'] else ""
            f.write(f"| {i} | `{type_name}`{namespace_info} | {info['count']:,} | {info['classification']} | {info['module']} |\n")

        # Module distribution
        f.write("\n## Module Distribution\n\n")
        module_counts = Counter(c['module'] for c in classifications.values())
        f.write("| Module | Type Count | Percentage |\n")
        f.write("|--------|------------|------------|\n")
        for module, count in sorted(module_counts.items(), key=lambda x: x[1], reverse=True):
            pct = count * 100 // unique_types
            f.write(f"| {module} | {count} | {pct}% |\n")

        # Classification by type
        f.write("\n## Classification Breakdown\n\n")
        f.write("### Enumerations\n\n")
        enum_types = [(name, info) for name, info in sorted_types if info['classification'] == 'enum']
        f.write(f"Total: {len(enum_types)} types\n\n")
        if enum_types:
            f.write("Top 20:\n")
            for name, info in enum_types[:20]:
                f.write(f"- `{name}` ({info['count']:,} occurrences, module: {info['module']})\n")

        f.write("\n### Namespaces\n\n")
        ns_types = [(name, info) for name, info in sorted_types if info['classification'] == 'namespace']
        f.write(f"Total: {len(ns_types)} types\n\n")
        if ns_types:
            f.write("Top 20:\n")
            for name, info in ns_types[:20]:
                f.write(f"- `{name}` (namespace: {info['namespace']}, {info['count']:,} occurrences, module: {info['module']})\n")

        f.write("\n### Classes/Structs\n\n")
        class_types = [(name, info) for name, info in sorted_types if info['classification'] == 'class/struct']
        f.write(f"Total: {len(class_types)} types\n\n")
        if class_types:
            f.write("Top 30:\n")
            for name, info in class_types[:30]:
                f.write(f"- `{name}` ({info['count']:,} occurrences, module: {info['module']})\n")

        # All types (for reference)
        f.write("\n## Complete Type List\n\n")
        f.write("<details>\n<summary>Click to expand (all types sorted by frequency)</summary>\n\n")
        f.write("| Type Name | Count | Classification | Module |\n")
        f.write("|-----------|-------|----------------|--------|\n")
        for type_name, info in sorted_types:
            f.write(f"| `{type_name}` | {info['count']:,} | {info['classification']} | {info['module']} |\n")
        f.write("\n</details>\n")

        # Next steps
        f.write("\n## Next Steps\n\n")
        f.write("1. **Extract enum definitions** - Focus on top 20 enums (quick wins)\n")
        f.write("2. **Define namespace structure** - Clarify namespace hierarchy\n")
        f.write("3. **Create class stubs** - Start with high-frequency classes\n")
        f.write("4. **Module-based approach** - Group fixes by module for better organization\n\n")
        f.write("**Recommended Order**:\n")
        f.write("1. Core types (StringUnorderedMap, basic containers)\n")
        f.write("2. Enums (ENUM_MODULE_TYPE, ENUM_INTERNALPACKET_*, etc.)\n")
        f.write("3. Network layer (packet types, session types)\n")
        f.write("4. Database layer (query types, handle types)\n")
        f.write("5. Game logic (character, inventory, dungeon types)\n")

    print(f"\nReport saved to: {output_file}")

    # Return summary for console output
    return {
        'total_errors': len(types),
        'unique_types': unique_types,
        'enum_count': enum_count,
        'namespace_count': namespace_count,
        'class_count': class_count,
        'top_5': sorted_types[:5]
    }

if __name__ == '__main__':
    log_file = Path('compile_after_syntax_fix_20251029_094736.log')
    output_file = Path('docs/analysis/PHASE1_TYPE_CLASSIFICATION.md')

    # Ensure output directory exists
    output_file.parent.mkdir(parents=True, exist_ok=True)

    # Generate report
    summary = generate_report(log_file, output_file)

    # Print summary
    print("\n" + "="*60)
    print("ANALYSIS SUMMARY")
    print("="*60)
    print(f"Total type reference errors: {summary['total_errors']:,}")
    print(f"Unique missing types: {summary['unique_types']:,}")
    print(f"  - Enumerations: {summary['enum_count']}")
    print(f"  - Namespaces: {summary['namespace_count']}")
    print(f"  - Classes/Structs: {summary['class_count']}")
    print(f"\nTop 5 Missing Types:")
    for i, (name, info) in enumerate(summary['top_5'], 1):
        print(f"  {i}. {name} ({info['count']:,} occurrences, {info['classification']}, module: {info['module']})")
    print(f"\nDetailed report: {output_file}")
    print("="*60)
