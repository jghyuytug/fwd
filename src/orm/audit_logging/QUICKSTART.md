# Audit Logging ORM Development Quick Guide

**1 Database Module | 101 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Encountered errors? → [Common Errors](#common-errors)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Audit Logging Module Overview

### Database Module (101 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `taiwan_cain_log/` | 101 | Game activity audit logs | `DB_TYPE_CAIN = 2` |

### Module Structure

```
taiwan_cain_log/
├── headers/    # ORM header files (.h)
├── sources/    # ORM source files (.c)
├── tests/      # Test files (test_*.c)
├── sql/        # SQL scripts (table structure)
└── docs/       # Module documentation
```

**Example**: Working on `charac_log` table
```
audit_logging/taiwan_cain_log/
├── headers/charac_log_orm.h
├── sources/charac_log_orm.c
├── tests/test_charac_log_orm.c
└── sql/charac_log.sql
```

---

## Audit Log Characteristics

Audit logging tables typically have:
- **AUTO_INCREMENT** primary key (log_id, seq_no, etc.)
- **DATETIME** fields for timestamps
- **Large VARCHAR** fields for detailed information
- **No UPDATE/DELETE** operations (append-only logs)
- **Time-based queries** (by date range)
- **Index on timestamp** columns for performance

---

## Development Templates

### Template 1: Basic Audit Log

**Typical Structure**:
```sql
CREATE TABLE charac_log (
  log_id bigint(20) NOT NULL auto_increment,
  m_id int(11) NOT NULL,
  charac_no int(11) NOT NULL,
  action_type varchar(50),
  action_detail text,
  action_time datetime,
  PRIMARY KEY (log_id),
  KEY idx_charac (charac_no),
  KEY idx_time (action_time)
);
```

**Header File** `charac_log_orm.h`:
```c
#ifndef CHARAC_LOG_ORM_H
#define CHARAC_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long log_id;  // bigint(20)
    int m_id;
    int charac_no;
    char action_type[51];
    char action_detail[1024];  // text → reasonable size
    char action_time[20];
} CharacLog;

/* Add Operations - Logs are append-only */
int CharacLog_Add(DBConnectionManager* manager, const CharacLog* log);

/* Query Operations */
int CharacLog_GetByCharac(DBConnectionManager* manager, int charac_no,
                          CharacLog* logs, int max_count, int* actual_count);
int CharacLog_GetByTimeRange(DBConnectionManager* manager,
                              const char* start_time, const char* end_time,
                              CharacLog* logs, int max_count, int* actual_count);
int CharacLog_GetByCharacAndTime(DBConnectionManager* manager,
                                  int charac_no,
                                  const char* start_time, const char* end_time,
                                  CharacLog* logs, int max_count, int* actual_count);

/* Utilities */
void CharacLog_PrintInfo(const CharacLog* log);

#endif
```

**Source File** `charac_log_orm.c`:
```c
#include "charac_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192  // Larger for TEXT fields

int CharacLog_Add(DBConnectionManager* manager, const CharacLog* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !log) return -1;

    // log_id is auto-generated, use NOW() if time not specified
    if (log->action_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO charac_log (m_id, charac_no, action_type, action_detail, action_time) "
            "VALUES (%d, %d, '%s', '%s', '%s')",
            log->m_id, log->charac_no, log->action_type, log->action_detail, log->action_time);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO charac_log (m_id, charac_no, action_type, action_detail, action_time) "
            "VALUES (%d, %d, '%s', '%s', NOW())",
            log->m_id, log->charac_no, log->action_type, log->action_detail);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CharacLog_GetByCharacAndTime(DBConnectionManager* manager,
                                  int charac_no,
                                  const char* start_time,
                                  const char* end_time,
                                  CharacLog* logs,
                                  int max_count,
                                  int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !logs || !actual_count || !start_time || !end_time) return -1;

    snprintf(query, sizeof(query),
        "SELECT log_id, m_id, charac_no, action_type, action_detail, action_time "
        "FROM charac_log "
        "WHERE charac_no = %d AND action_time BETWEEN '%s' AND '%s' "
        "ORDER BY action_time DESC LIMIT %d",
        charac_no, start_time, end_time, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&logs[count], 0, sizeof(CharacLog));
        logs[count].log_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;  // Use atoll for bigint
        logs[count].m_id = row[1] ? atoi(row[1]) : 0;
        logs[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(logs[count].action_type, row[3], sizeof(logs[count].action_type) - 1);
        if (row[4]) strncpy(logs[count].action_detail, row[4], sizeof(logs[count].action_detail) - 1);
        if (row[5]) strncpy(logs[count].action_time, row[5], sizeof(logs[count].action_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

### Template 2: Item Transaction Log

**Typical Structure**:
```sql
CREATE TABLE item_log (
  seq_no bigint(20) NOT NULL auto_increment,
  charac_no int(11) NOT NULL,
  item_id int(11) NOT NULL,
  item_name varchar(200),
  quantity int(11),
  transaction_type enum('add','remove','trade'),
  log_time datetime,
  PRIMARY KEY (seq_no),
  KEY idx_charac_time (charac_no, log_time)
);
```

**Key Implementation**:
```c
typedef struct {
    unsigned long long seq_no;
    int charac_no;
    int item_id;
    char item_name[201];
    int quantity;
    char transaction_type;  // 'a' = add, 'r' = remove, 't' = trade
    char log_time[20];
} ItemLog;

int ItemLog_Add(DBConnectionManager* manager, const ItemLog* log) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    const char* type_str;

    if (!manager || !log) return -1;

    // Convert char to enum string
    switch (log->transaction_type) {
        case 'a': type_str = "add"; break;
        case 'r': type_str = "remove"; break;
        case 't': type_str = "trade"; break;
        default: type_str = "add"; break;
    }

    if (log->log_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO item_log (charac_no, item_id, item_name, quantity, transaction_type, log_time) "
            "VALUES (%d, %d, '%s', %d, '%s', '%s')",
            log->charac_no, log->item_id, log->item_name, log->quantity, type_str, log->log_time);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO item_log (charac_no, item_id, item_name, quantity, transaction_type, log_time) "
            "VALUES (%d, %d, '%s', %d, '%s', NOW())",
            log->charac_no, log->item_id, log->item_name, log->quantity, type_str);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: BIGINT Type Handling
```c
// ❌ Wrong - using atoi for bigint
log->log_id = row[0] ? atoi(row[0]) : 0;

// ✅ Correct - using atoll for bigint(20)
log->log_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
```

### Error 2: TEXT Field Size
```c
// ❌ Wrong - TEXT needs large buffer
char detail[256];

// ✅ Correct - Allocate reasonable size for TEXT
char detail[1024];  // Or larger based on actual usage

// Or use dynamic allocation for very large TEXT fields
char* detail = malloc(4096);
```

### Error 3: Log Table CRUD Operations
```c
// ❌ Wrong - Logs should not be updated or deleted
int CharacLog_Update(DBConnectionManager* manager, const CharacLog* log);
int CharacLog_Delete(DBConnectionManager* manager, unsigned long long log_id);

// ✅ Correct - Logs are append-only
int CharacLog_Add(DBConnectionManager* manager, const CharacLog* log);
// Only Add and Query operations
```

### Error 4: Escaping Special Characters in Logs
```c
// ❌ Dangerous - SQL injection risk in log details
snprintf(query, sizeof(query),
    "INSERT INTO log (detail) VALUES ('%s')", log->detail);

// ✅ Better - Escape special characters or use prepared statements
// For production, implement mysql_real_escape_string() or use parameterized queries
// For development/testing with controlled data, ensure inputs are validated
```

---

## Standard Development Process

### Complete Workflow (9 Steps)

```
Step 0: Find table structure → All tables in taiwan_cain_log database
Step 1: Create test table → Create in test_taiwan_cain_log
Step 2: Create header file → audit_logging/taiwan_cain_log/headers/<table>_orm.h
Step 3: Create source file → audit_logging/taiwan_cain_log/sources/<table>_orm.c
Step 4: Create test file → audit_logging/taiwan_cain_log/tests/test_<table>_orm.c
Step 5: Upload files → scp to server
Step 6: Deploy files → cp to compilation directory
Step 7: Compile test → gcc compile
Step 8: Run test → Verify functionality
Step 9: Update status → Mark in audit_logging/ALL_DATABASES_TABLES.md
```

---

### Step 0: Find Table Structure

**Search in audit logging database**:
```bash
# All log tables are in taiwan_cain_log
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_log -e "SHOW TABLES" | grep -i <table_name>'

# Get table structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_log -e "SHOW CREATE TABLE <table_name>\\G"'
```

**Database Type**: `DB_TYPE_CAIN = 2`

---

### Step 1: Create Test Table

```bash
# Create test table in test_taiwan_cain_log
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_cain_log -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure from Step 0
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# Verify
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_cain_log -e "DESCRIBE <table>"'
```

---

### Step 2-4: Create Code Files

All files go under `taiwan_cain_log/`:
```
audit_logging/taiwan_cain_log/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

### Step 5-9: Upload, Deploy, Compile, Test, Update

```bash
# Upload
cd "C:\Users\waw\Desktop\local_orm"
scp "audit_logging/taiwan_cain_log/headers/<table>_orm.h" vm131:/tmp/dnf_new_orm/headers/
scp "audit_logging/taiwan_cain_log/sources/<table>_orm.c" vm131:/tmp/dnf_new_orm/sources/
scp "audit_logging/taiwan_cain_log/tests/test_<table>_orm.c" vm131:/tmp/dnf_new_orm/tests/

# Deploy, compile, test - same process
# ...

# Update audit_logging/ALL_DATABASES_TABLES.md
```

---

## Quick Development Script

**Automation script** `dev_audit_module.sh`:
```bash
#!/bin/bash
TABLE=$1

if [ -z "$TABLE" ]; then
    echo "Usage: ./dev_audit_module.sh <table_name>"
    echo "Example: ./dev_audit_module.sh charac_log"
    echo ""
    echo "Note: All audit logging tables are in taiwan_cain_log database"
    exit 1
fi

echo "=== Developing Audit Logging Module: taiwan_cain_log/$TABLE ==="

cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading files..."
scp "audit_logging/taiwan_cain_log/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "audit_logging/taiwan_cain_log/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "audit_logging/taiwan_cain_log/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
wait
echo "✓ Upload complete"

# Deploy
echo "[2/5] Deploying files..."
ssh vm131 "cp /tmp/dnf_new_orm/sources/${TABLE}_orm.c /tmp/dnf_new_orm/src/database/ && \
           cp /tmp/dnf_new_orm/headers/${TABLE}_orm.h /tmp/dnf_new_orm/include/database/headers/"
echo "✓ Deployment complete"

# Compile
echo "[3/5] Compiling..."
ssh vm131 "cd /tmp/dnf_new_orm/tests && gcc -m32 -g -Wall -Wextra -std=c99 \
    -I../include -I../include/database/headers -I../include/common -I../include/common/headers \
    -o test_${TABLE}_orm test_${TABLE}_orm.c \
    ../src/database/db_connection_manager.c ../src/database/${TABLE}_orm.c \
    -L/usr/lib/mysql -lmysqlclient -lpthread 2>&1"

[ $? -eq 0 ] && echo "✓ Compilation successful" || { echo "✗ Compilation failed"; exit 1; }

# Test
echo "[4/5] Running tests..."
ssh vm131 "/tmp/dnf_new_orm/tests/test_${TABLE}_orm /tmp/dnf_new_orm/config/database.cfg 2>&1 | grep -v POOL_DEBUG"

[ $? -eq 0 ] && echo "✓ Tests passed" || { echo "✗ Tests failed"; exit 1; }

echo "[5/5] Complete!"
echo ""
echo "Next: Update audit_logging/ALL_DATABASES_TABLES.md"
```

**Usage**:
```bash
chmod +x dev_audit_module.sh
./dev_audit_module.sh charac_log
./dev_audit_module.sh item_log
```

---

## Test File Template

```c
#include "charac_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_add_log(DBConnectionManager* manager);
void test_query_by_time(DBConnectionManager* manager);
void cleanup_test_data(DBConnectionManager* manager);

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&manager, 0, sizeof(DBConnectionManager));
    if (DBConnectionManager_Initialize(&manager, argv[1]) < 0) {
        printf("Failed to initialize connection manager\n");
        return 1;
    }

    /* Audit logging uses DB_TYPE_CAIN */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Audit Logging ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_add_log(&manager);
    test_query_by_time(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[512];

    snprintf(query, sizeof(query),
        "DELETE FROM charac_log WHERE charac_no IN (888001, 888002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}
```

---

## Quick Reference

### Database Type
```c
DB_TYPE_CAIN = 2  // taiwan_cain_log
```

### Test Database
```
test_taiwan_cain_log  // For all audit logging tables
```

### Type Mappings for Logs
```c
bigint(20)     → unsigned long long (use atoll)
text           → char[1024] or larger
datetime       → char[20]
enum           → char (single character)
```

### Development Checklist
```
□ Used DB_TYPE_CAIN for all operations
□ BIGINT fields use unsigned long long and atoll()
□ TEXT fields have adequate buffer size
□ Only Add and Query operations (no Update/Delete)
□ DATETIME queries use BETWEEN with quotes
□ Test files use English-only comments
□ Large query buffers (8192+) for TEXT fields
□ Time-based indexes utilized in queries
□ All results are freed
□ Test data cleaned up
```

---

## Best Practices for Audit Logs

### 1. Append-Only Operations
```c
// ✅ Good - Only add operations
int CharacLog_Add(DBConnectionManager* manager, const CharacLog* log);

// ❌ Bad - Never update or delete audit logs
// Audit logs should be immutable for compliance
```

### 2. Use Indexes for Time-based Queries
```c
// ✅ Good - Query with indexed timestamp column
SELECT * FROM charac_log
WHERE action_time BETWEEN '2025-11-01' AND '2025-11-30'
ORDER BY action_time DESC LIMIT 100;

// Query uses index on action_time
```

### 3. Limit Result Sets
```c
// ✅ Good - Always use LIMIT for log queries
int CharacLog_GetRecent(DBConnectionManager* manager,
                        CharacLog* logs, int max_count, int* actual_count) {
    snprintf(query, sizeof(query),
        "SELECT * FROM charac_log ORDER BY action_time DESC LIMIT %d", max_count);
    // ...
}
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\audit_logging`
- `taiwan_cain_log/` - Game activity audit logs (101 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
- Source database: `taiwan_cain_log`
- Test database: `test_taiwan_cain_log`
- User: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Audit Logging Specific Guide
