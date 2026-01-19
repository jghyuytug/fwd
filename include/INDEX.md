---
**Document Type**: Header Files and Type Definitions (19 Modules)
**Generated**: 2025-11-05
**Updated**: 2025-11-07
**Version**: v2.0 (Phase 5.8)
**Status**: 19 Module Headers Complete
---

# 📂 include/ - Header Files and Type Definitions Directory

## Overview

The `include/` directory contains all C header files defining data structures, interfaces, and type definitions for the 19-module modularized game server. This is the primary resource for understanding the codebase architecture and module interfaces.

**Subdirectories**: 19 module categories (Phase 5.8)
**Direct Header Files**: 8 core files (1+ MB)
**Total Files**: 60+ files
**Total Size**: 1.3+ MB (includes large constants.h)

---

## 📋 Quick Navigation

### 🔑 Core Definitions (Root Level)
- **defs.h** (7.1 KB) - IDA type definitions
- **constants.h** (1005 KB) - All magic numbers (extracted Phase 3)
- **constants_improved.h** (9.9 KB) - 34 optimized constants (Phase 3.6)
- **error_codes.h** (11 KB) - 79 error codes
- **all_generated.h** (3.6 KB) - Generated definitions

### 🏗️ Core Structures
- **CUser.h** (11 KB) - Player structure (45+ fields identified)
- **CInventory_v2.h** (5.6 KB) - Inventory structure (90% complete)
- **CInventory.h** (2.9 KB) - Original inventory

### 📦 Organized Modules (19 subdirectories - Phase 5.8)

---

## 📂 Directory Structure (19 Modules)

```
include/
├── 📄 Core Definitions (Root Level)
│   ├── defs.h (7.1 KB) ........................... IDA Type Defs
│   ├── constants.h (1005 KB) ..................... ALL Constants
│   ├── constants_improved.h (9.9 KB) ........... Phase 3.6 Constants
│   ├── error_codes.h (11 KB) .................... Error Codes
│   ├── all_generated.h (3.6 KB) ................ Generated Defs
│   ├── CUser.h (11 KB) ......................... Player Structure
│   ├── CInventory.h (2.9 KB) .................. Inventory v1
│   └── CInventory_v2.h (5.6 KB) ............... Inventory v2 (BEST)
│
├── 📂 Layer 0: Foundation
│   ├── common/                            # Common Headers
│   └── security/                          # Security & Anti-Cheat Types
│
├── 📂 Layer 1: Infrastructure
│   ├── database/                          # Database Types & Queries
│   └── network/                           # Network Protocol & Packets
│
├── 📂 Layer 2: Business Logic
│   ├── user/                              # Player & Character Data
│   ├── inventory/                         # Item, Equipment, Currency
│   └── events/                            # Event System Types
│
├── 📂 Layer 3: Game Systems (12 modules)
│   ├── combat/                            # Combat & Damage Structures
│   ├── dungeon/                           # Instance & Monster Types
│   ├── party/                             # Party System Types
│   ├── quest/                             # Quest Data Structures
│   ├── skill/                             # Skill Trees & Data
│   ├── guild/                             # Guild Management Types
│   ├── pvp/                               # PvP & Arena Types
│   ├── trade/                             # Trading System Types
│   ├── mail/                              # Mail System Types
│   ├── shop/                              # NPC Shop (Phase 5.8) ⭐
│   ├── enhancement/                       # Equipment Enhancement (Phase 5.8) ⭐
│   └── achievement/                       # Achievement System (Phase 5.8) ⭐
│
└── 📂 Legacy/Support (Reference Only)
    ├── lexer/                             # Lexer Components (reference)
    ├── parser/                            # Parser Components (reference)
    └── data/                              # Data Structure Utils (reference)
```

---

## 🔑 Core Header Files (Root Level)

### defs.h (7.1 KB)
**Purpose**: IDA/Hex-Rays type definitions
**Contains**:
- Standard IDA types (_DWORD, _BYTE, _WORD, etc.)
- Type compatibility definitions
- Pointer types
- Memory model definitions

**Usage**: Include for IDA type compatibility

### constants.h (1005 KB - LARGE!)
**Purpose**: Complete constant extraction (Phase 3 work)
**Contains**: 10,846+ constant definitions extracted from decompiled code
**Size Warning**: Very large file for reference only
**Best Practice**: Use constants_improved.h for most purposes

**Key Constants**:
- Game time constants (86400 = 1 day)
- Size limits (max inventory, max party)
- Network configurations
- Game logic thresholds
- Error values

### constants_improved.h (9.9 KB - RECOMMENDED!)
**Purpose**: Optimized constant definitions (Phase 3.6 work)
**Contains**: 34 core constants carefully selected and named
**Status**: Production-ready, recommended for use
**Replaces**: constants.h for most purposes

**Best Practices**:
- ✅ Use `constants_improved.h` in new code
- ⚠️ Reference `constants.h` for additional constants
- ✅ Add new constants here first

**Example Constants**:
```c
#define SECONDS_PER_DAY        86400
#define MAX_INVENTORY_SLOTS    697
#define MAX_PARTY_SIZE         4
#define ERROR_INVALID_USER     -1
```

### error_codes.h (11 KB)
**Purpose**: Standardized error codes
**Contains**: 79 error code definitions extracted (Phase 3)
**Status**: Complete extraction

**Categories**:
- Authentication errors (20+)
- Database errors (15+)
- Network errors (10+)
- Game logic errors (34+)

**Usage**: Always use defined error codes instead of magic numbers

### all_generated.h (3.6 KB)
**Purpose**: Auto-generated type and constant definitions

### system_init.h / system_init_framework.h / system_symbols.h
**Purpose**: System initialization and framework files

---

## 🏗️ Major Structures

### CUser.h (11 KB) ⭐ IMPORTANT

**Purpose**: Main player/character structure
**Status**: 45+ fields identified (out of 500+ estimated)
**Completion**: Phase 4 work
**Size**: Represents 584KB structure in decompiled code

**Key Sections**:
- Player identity (UID, name, level)
- Character progression
- Inventory references
- Skill data
- Quest progress
- PvP statistics
- Account information

**Usage**: Reference for understanding player data model

**Next Steps**: Identify remaining 455+ fields

### CInventory_v2.h (5.6 KB) - RECOMMENDED

**Purpose**: Optimized inventory structure
**Status**: 90% complete
**Completion**: Phase 4 work
**Version**: v2 is best version

**Key Components**:
- 697 item slots
- 4 currency types (gold, silver, etc.)
- Read/Write buffer separation
- Creature/avatar management

**Usage**: Use v2 in new code

**Alternative**: CInventory.h (older version)

---

## 📦 Module-Organized Headers

### common/
**Contents**: Common type definitions, utilities
**Related Source**: src/common/

### database/
**Contents**: Database-related types, query structures
**Related Source**: src/database/

### network/
**Contents**: Protocol definitions, packet structures
**Related Source**: src/network/
**Key Files**: May contain packet definitions

### user/
**Contents**: Player, character, account types
**Related Source**: src/user/

### inventory/
**Contents**: Item, inventory, storage types
**Related Source**: src/inventory/

### combat/
**Contents**: Damage calculation, status effects, buffs
**Related Source**: src/combat/
**Key Types**: Damage formula, status effects, buff tracking

### dungeon/
**Contents**: Instance, room, dungeon types
**Related Source**: src/dungeon/
**Key Types**: Dungeon instances, clearance tracking, monster data

### party/
**Contents**: Party system, group matching
**Related Source**: src/party/
**Key Types**: Party state, member tracking, matching data

### quest/
**Contents**: Quest data structures and progression
**Related Source**: src/quest/
**Key Types**: Quest state, objectives, rewards

### skill/
**Contents**: Skill trees and definitions
**Related Source**: src/skill/
**Key Types**: Skill trees, upgrades, execution data

### guild/
**Contents**: Guild management and member types
**Related Source**: src/guild/
**Key Types**: Guild data, member roles, permissions

### pvp/
**Contents**: PvP ranking and arena types
**Related Source**: src/pvp/
**Key Types**: PvP rankings, arena data, points

### trade/
**Contents**: Trading system types
**Related Source**: src/trade/
**Key Types**: Trade requests, item verification

### mail/
**Contents**: Mail and messaging system types
**Related Source**: src/mail/
**Key Types**: Mail data, attachments, notifications

### shop/
**Contents**: NPC shop and merchant data (Phase 5.8) ⭐
**Related Source**: src/shop/
**Key Types**: Shop inventory, pricing, refresh logic

### enhancement/
**Contents**: Equipment enhancement and upgrade types (Phase 5.8) ⭐
**Related Source**: src/enhancement/
**Key Types**: Enhancement levels, success rates, materials

### achievement/
**Contents**: Achievement tracking and reward types (Phase 5.8) ⭐
**Related Source**: src/achievement/
**Key Types**: Achievement data, progress tracking, categories

### security/
**Contents**: Anti-cheat and validation types
**Related Source**: src/security/
**Key Types**: Hack detection, GM accounts, IP management

### events/
**Contents**: Event system type definitions
**Related Source**: src/events/
**Key Types**: Event classes, dispatchers, handlers

### common/
**Contents**: Common type definitions and utilities
**Related Source**: src/common/
**Key Types**: Shared utilities, helper structures

### database/
**Contents**: Database-related types and queries
**Related Source**: src/database/
**Key Types**: Query structures, connection data

### network/
**Contents**: Network protocol and packet definitions
**Related Source**: src/network/
**Key Types**: Packet structures, protocol definitions

### lexer/ & parser/
**Contents**: Lexer and parser component types (reference)
**Related Source**: lexer/, parser/
**Status**: Legacy support files

### data/ & legacy/
**Contents**: Old data structures and deprecated definitions
**Status**: Reference only, not used in Phase 5.8 modules

---

## 📊 Header Statistics (19 Modules)

| Layer | File/Directory | Size | Purpose | Priority |
|-------|---|---|---|---|
| 0 | common/ | - | Utilities | HIGH |
| 0 | security/ | - | Anti-cheat types | HIGH |
| 1 | database/ | - | DB types | HIGH |
| 1 | network/ | - | Protocol types | HIGH |
| 2 | user/ | 11 KB | Player structure | HIGH |
| 2 | inventory/ | 5.6 KB | Inventory types | HIGH |
| 2 | events/ | - | Event types | MEDIUM |
| 3 | combat/ | - | Combat types | MEDIUM |
| 3 | dungeon/ | - | Dungeon types | MEDIUM |
| 3 | party/ | - | Party types | MEDIUM |
| 3 | quest/ | - | Quest types | MEDIUM |
| 3 | skill/ | - | Skill types | MEDIUM |
| 3 | guild/ | - | Guild types | MEDIUM |
| 3 | pvp/ | - | PvP types | MEDIUM |
| 3 | trade/ | - | Trade types | MEDIUM |
| 3 | mail/ | - | Mail types | LOW |
| 3 | shop/ | - | Shop types ⭐ | LOW |
| 3 | enhancement/ | - | Enhancement types ⭐ | LOW |
| 3 | achievement/ | - | Achievement types ⭐ | LOW |
| Core | constants_improved.h | 9.9 KB | Core constants | HIGH |
| Core | error_codes.h | 11 KB | Error codes | HIGH |
| Legacy | defs.h, constants.h | 1012 KB | Reference only | LOW |

---

## 🎯 Usage Scenarios

### Scenario 1: Understanding Player Data
```bash
cat include/CUser.h                    # View player structure
grep "offset" include/CUser.h         # Find specific fields
# Use to understand player attributes
```

### Scenario 2: Working with Inventory
```bash
cat include/CInventory_v2.h           # View inventory structure
# Use v2 for new inventory features
```

### Scenario 3: Using Constants
```c
// In source code
#include "include/constants_improved.h"

if (inventory_slots > MAX_INVENTORY_SLOTS) {
    return ERROR_INVENTORY_FULL;
}
```

### Scenario 4: Error Handling
```c
#include "include/error_codes.h"

if (status == ERROR_INVALID_USER) {
    // Handle invalid user
}
```

### Scenario 5: Network Development
```bash
grep -r "packet" include/network/     # Find packet definitions
# Use to understand protocol
```

---

## 📈 Structure Recovery Progress

### Completed Structures
- ✅ CUser (45+ fields / 500+ total)
- ✅ CInventory (697 slots identified, 90% complete)
- ✅ Error codes (79 codes extracted)
- ✅ Constants (10,846 definitions extracted, 34 optimized)

### In Progress
- 🔄 CUser (Need 455+ more fields)
- 🔄 Game logic structures
- 🔄 Network protocol details

### Potential Future Work
- 📋 CDungeon structure analysis
- 📋 CParty structure analysis
- 📋 Item system structures
- 📋 Quest system structures

---

## ✅ Development Checklist

When Using Headers:
- [ ] Include correct header file
- [ ] Use constants_improved.h (not constants.h)
- [ ] Reference CInventory_v2.h (not v1)
- [ ] Check error_codes.h for error values
- [ ] Understand structure offsets in CUser.h
- [ ] Keep headers in sync with source code

---

## 🔗 Related Files

**Source Code**: `src/` directory mirrors header organization
**Documentation**: `docs/analysis/` contains structure analysis
**Phase Reports**: `docs/phases/PHASE4_STRUCT_RECOVERY_COMPLETE.md`

---

## 💡 Best Practices

1. **Use constants_improved.h** - Better names, smaller file
2. **Reference CUser.h** - Understand player model
3. **Study CInventory_v2.h** - For inventory work
4. **Check error_codes.h** - Don't use magic numbers
5. **Keep in sync** - Update headers when code changes
6. **Document structures** - Add comments to explain fields
7. **Version tracking** - Mark structure versions

---

## 🚀 Quick Start

### Find a Type Definition
```bash
grep -r "typedef.*MyType" include/
```

### Find a Structure
```bash
grep -r "struct CMyClass" include/
```

### Find a Constant
```bash
grep -r "CONSTANT_NAME" include/constants_improved.h
```

### Add New Header
```bash
# Create in appropriate subdirectory
touch include/game/mynewtype.h
# Add to appropriate section of code
```

---

**Directory Type**: Header Files and Type Definitions (19 Modules)
**Organization**: By 4-layer architecture and module system
**Total Headers**: 60+ files
**Total Size**: 1.3+ MB
**Last Updated**: 2025-11-07 (Phase 5.8)
**Status**: Phase 5.8 Complete - All 19 module headers organized

**Phase 5.8 Additions**:
- shop/ - NPC shop and merchant data headers
- enhancement/ - Equipment enhancement system headers
- achievement/ - Achievement tracking and reward headers
- 4-layer architecture organization (Foundation → Infrastructure → Business Logic → Game Systems)

**Next Steps**:
- Use corresponding headers for Phase 5.8 module development
- Reference CUser.h for player data model
- Use constants_improved.h for new constants
- Check module-specific headers for structure definitions
