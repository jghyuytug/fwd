# Core Game ORM Development Quick Guide

**5 Database Modules | 288 Tables | Current Progress: 97.9%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process) (9-step complete workflow)
- Need code templates? → [Template 1: Simple Table](#template-1-simple-table-single-primary-key)
- Encountered compilation errors? → [Common Errors](#common-errors-verified) | [Troubleshooting](#troubleshooting)
- Don't know type mapping? → [Field Type Mapping](#field-type-mapping)
- Need command reference? → [Actual Workflow](#actual-workflow-command-reference)

---

## Core Game Module Overview

### 5 Database Modules (288 Tables)

| Database Module | Tables | Progress | DB Type Constant |
|----------------|--------|----------|------------------|
| `taiwan_cain/` | 85 | High | `DB_TYPE_CAIN = 2` |
| `d_taiwan/` | 114 | High | `DB_TYPE_TAIWAN = 1` |
| `taiwan_login/` | 51 | High | `DB_TYPE_LOGIN = 4` |
| `d_guild/` | 26 | High | `DB_TYPE_GUILD = 5` |
| `taiwan_billing/` | 12 | High | `DB_TYPE_BILLING = 3` |

### Module Structure

Each database module contains:
```
<database>/
├── headers/    # ORM header files (.h)
├── sources/    # ORM source files (.c)
├── tests/      # Test files (test_*.c)
├── sql/        # SQL scripts (table structure)
└── docs/       # Module documentation
```

**Example**: Working on `taiwan_cain/charac_info` table
```
core_game/taiwan_cain/
├── headers/charac_info_orm.h
├── sources/charac_info_orm.c
├── tests/test_charac_info_orm.c
└── sql/charac_info.sql
```

---

## Development Templates

### Template 1: Simple Table (Single Primary Key)

**Example**: `member_info`
```sql
CREATE TABLE member_info (
  m_id int(11) NOT NULL,
  m_name varchar(50) NOT NULL,
  m_level int(11) NOT NULL DEFAULT 1,
  PRIMARY KEY (m_id)
);
```

**Header File** `member_info_orm.h`:
```c
#ifndef MEMBER_INFO_ORM_H
#define MEMBER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char m_name[51];
    int m_level;
} MemberInfo;

/* CRUD Operations */
int MemberInfo_Add(DBConnectionManager* manager, const MemberInfo* member);
int MemberInfo_Get(DBConnectionManager* manager, int m_id, MemberInfo* member);
int MemberInfo_Update(DBConnectionManager* manager, const MemberInfo* member);
int MemberInfo_Delete(DBConnectionManager* manager, int m_id);
int MemberInfo_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int MemberInfo_GetAll(DBConnectionManager* manager, MemberInfo* members,
                      int max_count, int* actual_count);

/* Utilities */
void MemberInfo_PrintInfo(const MemberInfo* member);

#endif
```

**Source File** `member_info_orm.c`:
```c
#include "member_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberInfo_Get(DBConnectionManager* manager, int m_id, MemberInfo* member) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];  // Note: Number of fields

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_name, m_level FROM member_info WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {  // Note: <= 0
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(member, 0, sizeof(MemberInfo));
    member->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(member->m_name, row[1], sizeof(member->m_name) - 1);
    member->m_level = row[2] ? atoi(row[2]) : 1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

### Template 2: Composite Primary Key

**Example**: `restrict_state`
```sql
CREATE TABLE restrict_state (
  server_group int(11) NOT NULL,
  category int(11) NOT NULL,
  restrict_code int(11) NOT NULL,
  PRIMARY KEY (server_group, category, restrict_code)
);
```

**Key Differences**:
```c
/* Get/Delete/Exists require all primary keys */
int RestrictState_Get(DBConnectionManager* manager,
                      int server_group,     // Primary key 1
                      int category,         // Primary key 2
                      int restrict_code,    // Primary key 3
                      RestrictState* state);

/* WHERE must include all primary keys */
snprintf(query, sizeof(query),
    "SELECT ... FROM restrict_state "
    "WHERE server_group = %d AND category = %d AND restrict_code = %d",
    server_group, category, restrict_code);
```

---

### Template 3: AUTO_INCREMENT Primary Key

**Example**: `charac_info` (taiwan_cain)
```sql
CREATE TABLE charac_info (
  charac_no int(11) NOT NULL auto_increment,
  m_id int(11) NOT NULL,
  charac_name varchar(50),
  reg_date datetime,
  PRIMARY KEY (charac_no),
  UNIQUE KEY (m_id, charac_name)
);
```

**Key Handling**:
```c
/* Add function does not need to pass id */
int CharacInfo_Add(DBConnectionManager* manager, const CharacInfo* charac) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    // charac_no is auto-generated by database, not specified in INSERT
    // DATETIME handling - supports custom time and NOW()
    if (charac->reg_date[0]) {
        // User-provided time
        snprintf(query, sizeof(query),
            "INSERT INTO charac_info (m_id, charac_name, reg_date) "
            "VALUES (%d, '%s', '%s')",
            charac->m_id, charac->charac_name, charac->reg_date);
    } else {
        // Use database current time
        snprintf(query, sizeof(query),
            "INSERT INTO charac_info (m_id, charac_name, reg_date) "
            "VALUES (%d, '%s', NOW())",
            charac->m_id, charac->charac_name);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors (Verified)

### Error 1: Chinese Comments in Test Files
**Symptom**: Compilation error `error: stray '\346' in program`
```c
/* 测试总数 */           // ❌ Chinese comments cause compilation failure
static int test_total;

/* Test total */         // ✅ Must use English comments
static int test_total;
```
**Solution**: **NO Chinese characters** in test .c files (including comments, strings)

---

### Error 2: Wrong API Function Names
**Symptom**: `warning: implicit declaration of function 'DBConnectionManager_Query'`
```c
// ❌ Wrong API
DBConnectionManager_Query(manager, DB_TYPE_TAIWAN, query, &result);
DBConnectionManager_Execute(manager, DB_TYPE_TAIWAN, query);

// ✅ Correct API
DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
```

---

### Error 3: Wrong Initialization Method
**Symptom**: `error: too many arguments to function 'DBConnectionManager_Connect'`
```c
// ❌ Wrong initialization (non-existent API)
DBConnectionManager* manager = DBConnectionManager_Create();
DBConnectionManager_Connect(manager, DB_TYPE_TAIWAN, host, user, pass, db, port);
DBConnectionManager_Destroy(manager);

// ✅ Correct initialization
DBConnectionManager manager;
memset(&manager, 0, sizeof(DBConnectionManager));
DBConnectionManager_Initialize(&manager, config_file);
DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN);
DBConnectionManager_Cleanup(&manager);
```

---

### Error 4: VARCHAR Array Size
```c
char m_name[50];  // ❌ Missing '\0' space
char m_name[51];  // ✅ Correct (VARCHAR(50) → 51)
```

---

### Error 5: Loop Condition Error
```c
while (DBQueryResult_FetchRow(&result, row) >= 0)  // ❌ Will loop one extra time
while (DBQueryResult_FetchRow(&result, row) > 0)   // ✅ Correct
```

---

### Error 6: Forgot memset Initialization
```c
MemberInfo member;                       // ❌ Uninitialized
member.m_id = 1;

MemberInfo member;                       // ✅ Clear first
memset(&member, 0, sizeof(MemberInfo));
member.m_id = 1;
```

---

### Error 7: Forgot to Free Result
```c
DBConnectionManager_ExecuteQuery(..., &result);  // ❌ Memory leak
return 0;

DBConnectionManager_ExecuteQuery(..., &result);  // ✅ Must free
DBQueryResult_Free(&result);
return 0;
```

---

## Field Type Mapping

| MySQL Type | C Type | Array Size | Notes |
|------------|--------|------------|-------|
| `INT(11)` | `int` | - | `atoi()` conversion |
| `TINYINT(4)` | `unsigned char` | - | 0-255 |
| `VARCHAR(N)` | `char[]` | **N+1** | Note: **+1 for '\0'** |
| `DATETIME` | `char[20]` | 20 | 'YYYY-MM-DD HH:MM:SS' |
| `ENUM('a','b')` | `char` | - | Single character 'a' or 'b' |
| `BINARY(N)` | `unsigned char[]` | N | Fixed length |

### Key Rules

```c
// VARCHAR(50) → char[51]
char m_name[51];  // 50 + 1 for '\0'

// Safe string copy
strncpy(dst, src, sizeof(dst) - 1);
dst[sizeof(dst) - 1] = '\0';

// ENUM use character comparison
if (flag == 'y')  // ✅
if (flag == "y")  // ❌ Compilation error
```

---

## Standard Development Process

### Complete Workflow (9 Steps)

```
Step 0: Find table structure → Must determine which database the table is in
Step 1: Create test table → Create table in test database
Step 2: Create header file → Define struct and functions
Step 3: Create source file → Implement all functions
Step 4: Create test file → Write test cases (English only)
Step 5: Upload files → scp to server
Step 6: Deploy files → cp to compilation directory
Step 7: Compile test → gcc compile
Step 8: Run test → Verify functionality
Step 9: Update status → Mark as completed
```

---

### Step 0: Find Table Structure (First Step)

**Important**: Must search first, cannot guess table structure or database location.

**Search Commands**:
```bash
# Search database by database (recommended)
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 d_taiwan -e "SHOW TABLES" | grep -i <table_name>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain -e "SHOW TABLES" | grep -i <table_name>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_login -e "SHOW TABLES" | grep -i <table_name>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 d_guild -e "SHOW TABLES" | grep -i <table_name>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_billing -e "SHOW TABLES" | grep -i <table_name>'
```

**Get Table Structure**:
```bash
# After finding the table, get complete definition
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table_name>\\G"'
```

**Database Type Mapping**:
```c
d_taiwan       → DB_TYPE_TAIWAN  = 1
taiwan_cain    → DB_TYPE_CAIN    = 2
taiwan_billing → DB_TYPE_BILLING = 3
taiwan_login   → DB_TYPE_LOGIN   = 4
d_guild        → DB_TYPE_GUILD   = 5
```

---

### Step 1: Create Test Table

```bash
# Create test table based on structure from Step 0
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_<db> -e "
  CREATE TABLE IF NOT EXISTS <table> (
    m_id int(11) NOT NULL default 0,
    m_name varchar(50) NOT NULL,
    PRIMARY KEY (m_id)
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# Verify table created
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_<db> -e "DESCRIBE <table>"'
```

---

### Step 2-4: Create Code Files

These steps are done locally in the core_game directory structure:
- Step 2: Create `<database>/headers/<table>_orm.h` (struct definition, function declarations)
- Step 3: Create `<database>/sources/<table>_orm.c` (function implementations)
- Step 4: Create `<database>/tests/test_<table>_orm.c` (test cases, **English only**)

**File Paths Example** (taiwan_cain database):
```
core_game/taiwan_cain/
├── headers/member_info_orm.h
├── sources/member_info_orm.c
└── tests/test_member_info_orm.c
```

---

### Step 5-6: Upload and Deploy

```bash
# Step 5: Upload files (adjust path based on database module)
cd "C:\Users\waw\Desktop\local_orm"
scp "core_game/taiwan_cain/headers/<table>_orm.h" vm131:/tmp/dnf_new_orm/headers/
scp "core_game/taiwan_cain/sources/<table>_orm.c" vm131:/tmp/dnf_new_orm/sources/
scp "core_game/taiwan_cain/tests/test_<table>_orm.c" vm131:/tmp/dnf_new_orm/tests/

# Step 6: Deploy to compilation directory
ssh vm131 "cp /tmp/dnf_new_orm/sources/<table>_orm.c /tmp/dnf_new_orm/src/database/ && \
           cp /tmp/dnf_new_orm/headers/<table>_orm.h /tmp/dnf_new_orm/include/database/headers/"
```

---

### Step 7-8: Compile and Test

```bash
# Step 7: Compile
ssh vm131 "cd /tmp/dnf_new_orm/tests && gcc -m32 -g -Wall -Wextra -std=c99 \
    -I../include -I../include/database/headers -I../include/common -I../include/common/headers \
    -o test_<table>_orm test_<table>_orm.c \
    ../src/database/db_connection_manager.c ../src/database/<table>_orm.c \
    -L/usr/lib/mysql -lmysqlclient -lpthread"

# Step 8: Run test
ssh vm131 "/tmp/dnf_new_orm/tests/test_<table>_orm /tmp/dnf_new_orm/config/database.cfg 2>&1 | grep -v POOL_DEBUG"
```

---

### Step 9: Update Status Document

Update `core_game/ALL_DATABASES_TABLES.md` locally:
```markdown
# Before
- ❌ member_info - Member information

# After
- ✅ member_info - Member information
```

Also update statistics (completed count +1, pending count -1).

---

## Actual Workflow (Command Reference)

### Quick Development Script

**Complete automation script** `dev_core_game_module.sh` (local Git Bash):
```bash
#!/bin/bash
DATABASE=$1  # taiwan_cain, d_taiwan, taiwan_login, d_guild, taiwan_billing
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_core_game_module.sh <database> <table_name>"
    echo "Example: ./dev_core_game_module.sh taiwan_cain charac_info"
    echo ""
    echo "Available databases:"
    echo "  - taiwan_cain"
    echo "  - d_taiwan"
    echo "  - taiwan_login"
    echo "  - d_guild"
    echo "  - taiwan_billing"
    exit 1
fi

echo "=== Developing Core Game Module: $DATABASE/$TABLE ==="

# Step 1: Upload files
echo "[1/5] Uploading files to server..."
cd "C:/Users/waw/Desktop/local_orm"
scp "core_game/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "core_game/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "core_game/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
wait
echo "✓ Upload complete"

# Step 2: Deploy files
echo "[2/5] Deploying files to compilation directory..."
ssh vm131 "cp /tmp/dnf_new_orm/sources/${TABLE}_orm.c /tmp/dnf_new_orm/src/database/ && \
           cp /tmp/dnf_new_orm/headers/${TABLE}_orm.h /tmp/dnf_new_orm/include/database/headers/"
echo "✓ Deployment complete"

# Step 3: Compile
echo "[3/5] Compiling test program..."
ssh vm131 "cd /tmp/dnf_new_orm/tests && gcc -m32 -g -Wall -Wextra -std=c99 \
    -I../include -I../include/database/headers -I../include/common -I../include/common/headers \
    -o test_${TABLE}_orm test_${TABLE}_orm.c \
    ../src/database/db_connection_manager.c ../src/database/${TABLE}_orm.c \
    -L/usr/lib/mysql -lmysqlclient -lpthread 2>&1"

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful"
else
    echo "✗ Compilation failed"
    exit 1
fi

# Step 4: Run test
echo "[4/5] Running tests..."
ssh vm131 "/tmp/dnf_new_orm/tests/test_${TABLE}_orm /tmp/dnf_new_orm/config/database.cfg 2>&1 | grep -v POOL_DEBUG"

if [ $? -eq 0 ]; then
    echo "✓ Tests passed"
else
    echo "✗ Tests failed"
    exit 1
fi

# Step 5: Prompt for status update
echo "[5/5] Complete!"
echo ""
echo "Next step: Update core_game/ALL_DATABASES_TABLES.md"
echo "  - Mark table as ✅"
echo "  - Update progress statistics"
```

**Usage**:
```bash
# Make script executable
chmod +x dev_core_game_module.sh

# Run development script
./dev_core_game_module.sh taiwan_cain charac_info
./dev_core_game_module.sh d_taiwan member_info
```

---

## Test File Standard Template

### Basic Test Framework
```c
#include "table_name_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* IMPORTANT: NO Chinese characters in test files! */
void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    /* Initialize connection manager */
    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Connect to database - use appropriate DB_TYPE_* */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Table Name ORM Test Suite ===\n\n");

    /* Clean up before test */
    cleanup_test_data(&manager);

    /* Run tests */
    test_basic_crud(&manager);
    test_business_operations(&manager);

    /* Clean up after test */
    cleanup_test_data(&manager);

    /* Release resources */
    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM table_name WHERE m_id IN (100001, 200001)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
```

---

## Debugging Tips

### 1. View Actual SQL Statement
Print SQL before execution:
```c
snprintf(query, sizeof(query), "SELECT ...");
printf("DEBUG SQL: %s\n", query);  // Debug output
if (DBConnectionManager_ExecuteQuery(...) < 0) {
    printf("ERROR: Query failed\n");
    return -1;
}
```

### 2. Check Return Values
```c
int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
printf("DEBUG: ExecuteQuery returned %d\n", ret);
if (ret < 0) {
    printf("ERROR: Query execution failed\n");
    return -1;
}
```

### 3. Verify Row Data
```c
if (DBQueryResult_FetchRow(&result, row) > 0) {
    printf("DEBUG: row[0]=%s, row[1]=%s\n",
           row[0] ? row[0] : "NULL",
           row[1] ? row[1] : "NULL");
}
```

### 4. Filter Debug Output
```bash
# Filter out connection pool debug messages during runtime
./test_table_orm config.cfg 2>&1 | grep -v POOL_DEBUG
```

---

## Quick Reference

### Database Types
```c
DB_TYPE_TAIWAN  = 1  // d_taiwan
DB_TYPE_CAIN    = 2  // taiwan_cain
DB_TYPE_BILLING = 3  // taiwan_billing
DB_TYPE_LOGIN   = 4  // taiwan_login
DB_TYPE_GUILD   = 5  // d_guild
```

### Development Checklist

**Code Standards**:
```
□ Test files must have English-only comments/strings
□ Use DBConnectionManager_ExecuteQuery (not Query/Execute)
□ Use char* row[] (not char* values[])
□ Initialize with memset + Initialize + Connect (not Create)
□ VARCHAR(N) → char[N+1]
□ String copy has null terminator
□ Composite primary keys include all fields
□ Loop condition uses > 0 (not >= 0)
□ All pointers checked for NULL
□ All queries checked for < 0
□ All results are Free'd
□ Use memset to initialize structs
□ Test data is cleaned up
```

---

## Troubleshooting

### Compilation Errors

**Error**: `stray '\346' in program`
- **Cause**: Test file contains Chinese characters
- **Solution**: Remove all Chinese comments and strings

**Error**: `undefined reference to 'DBConnectionManager_Create'`
- **Cause**: Using non-existent API function
- **Solution**: Use `DBConnectionManager_Initialize()` + `Connect()`

### Runtime Errors

**Error**: `Query failed: You have an error in your SQL syntax`
- **Check**: Print SQL statement to view syntax
- **Common causes**:
  - DATETIME without quotes: `'2025-11-17 10:00:00'`
  - VARCHAR field without quotes: `'value'`
  - Wrong conditional expression

**Error**: Insert fails but no error message
- **Check**: Unique key constraint violation
- **Solution**: Clean up test data or use different test IDs

### Database Connection Issues

**Error**: `Failed to connect to database`
- **Check**: `database.cfg` configuration file path is correct
- **Check**: MySQL server is running: `mysql -h192.168.200.131 -uroot -p88888888`
- **Check**: Test database exists: `SHOW DATABASES LIKE 'test_%'`

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\core_game`
- `<database>/headers/` - Header files
- `<database>/sources/` - Source files (implementation)
- `<database>/tests/` - Test files
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
- SSH alias: `vm131` (key exchange configured)
- Compilation directory: `/tmp/dnf_new_orm/tests/`
- Header directory: `/tmp/dnf_new_orm/include/database/headers/`
- Config file: `/tmp/dnf_new_orm/config/database.cfg`

**Database**: MySQL 5.0.67 @ 192.168.200.131:3306
- Test databases: `test_d_taiwan`, `test_taiwan_cain`, `test_taiwan_login`, `test_d_guild`, `test_taiwan_billing`
- User: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Core Game Specific Guide
