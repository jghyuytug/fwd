#!/usr/bin/env python3
"""
add_forward_decl.py - Add forward declarations for namespaces
Reduces "not declared" errors by adding proper forward declarations
"""

import sys
import re

def main():
    namespace = sys.argv[sys.argv.index('--namespace') + 1] if '--namespace' in sys.argv else 'all'

    file_path = 'src_split/df_game_r_decl.h'

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File not found: {file_path}")
        sys.exit(1)

    # Define forward declarations for each namespace
    forward_decls = {
        'ARAD': '''
// Forward declarations for ARAD namespace
namespace ARAD {
  class CPlayer;
  class CItem;
  class CSkill;
  class CQuest;
  class CDungeon;
  namespace DATABASE {
    class CQuery;
  }
}
''',
        'WongWork': '''
// Forward declarations for WongWork namespace
namespace WongWork {
  class CHackAnalyzer;
  class CAutoPunishRuleBlackIPMgr;
  class CGMAccounts;
}
''',
        'QuickParty': '''
// Forward declarations for QuickParty namespace
namespace QuickParty {
  class CQuickPartySystemManager;
}
''',
        'others': '''
// Forward declarations for other namespaces
namespace exchange_server {
  class CEpoll;
  class CSession;
}

namespace EventClassify {
  class CEventScriptMng;
  class CEventActionMng;
}

namespace nexon {
  namespace cash {
    class CCashShop;
  }
}

namespace Taiwan {
  class CCashQuery;
}
'''
    }

    if namespace in forward_decls:
        decl = forward_decls[namespace]

        # Check if already exists
        if namespace in content and 'Forward declarations for' in content:
            print(f"⚠ {namespace} 前向声明已存在，跳过")
            return

        # Insert at the beginning (after include guards if any)
        lines = content.split('\n')
        insert_pos = 0

        # Find position after #ifndef/#define
        for i, line in enumerate(lines):
            if line.strip().startswith('#define') and '_DECL_H' in line.upper():
                insert_pos = i + 1
                break

        # Insert forward declarations
        lines.insert(insert_pos, decl)
        content = '\n'.join(lines)

        # Write output
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)

        print(f"✓ 已添加 {namespace} 命名空间前向声明")
    else:
        print(f"Error: Unknown namespace: {namespace}")
        sys.exit(1)

if __name__ == '__main__':
    main()
