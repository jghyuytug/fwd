# Security Monitoring ORM Development Quick Guide

**2 Database Modules | 50 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Encountered compilation errors? → [Common Errors](#common-errors)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Security Monitoring Module Overview

### 2 Database Modules (50 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `d_taiwan_secu/` | 8 | Security monitoring | `DB_TYPE_TAIWAN = 1` |
| `d_technical_report/` | 42 | Technical reports & performance monitoring | `DB_TYPE_TAIWAN = 1` |

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

**Example**: Working on `d_taiwan_secu/security_log` table
```
security_monitoring/d_taiwan_secu/
├── headers/security_log_orm.h
├── sources/security_log_orm.c
├── tests/test_security_log_orm.c
└── sql/security_log.sql
```

---

## Development Templates

### Template 1: Security Log Table (Time-based)

**Typical Structure**:
```sql
CREATE TABLE security_log (
  log_id int(11) NOT NULL auto_increment,
  m_id int(11) NOT NULL,
  event_type varchar(50),
  event_time datetime,
  ip_address varchar(45),
  PRIMARY KEY (log_id),
  KEY idx_m_id (m_id),
  KEY idx_event_time (event_time)
);
```

**Header File** `security_log_orm.h`:
```c
#ifndef SECURITY_LOG_ORM_H
#define SECURITY_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int log_id;
    int m_id;
    char event_type[51];
    char event_time[20];
    char ip_address[46];
} SecurityLog;

/* CRUD Operations */
int SecurityLog_Add(DBConnectionManager* manager, const SecurityLog* log);
int SecurityLog_Get(DBConnectionManager* manager, unsigned int log_id, SecurityLog* log);

/* Query Operations */
int SecurityLog_GetByMemberId(DBConnectionManager* manager, int m_id,
                               SecurityLog* logs, int max_count, int* actual_count);
int SecurityLog_GetByTimeRange(DBConnectionManager* manager,
                                const char* start_time, const char* end_time,
                                SecurityLog* logs, int max_count, int* actual_count);
int SecurityLog_GetByEventType(DBConnectionManager* manager, const char* event_type,
                                 SecurityLog* logs, int max_count, int* actual_count);

/* Utilities */
void SecurityLog_PrintInfo(const SecurityLog* log);

#endif
```

---

### Template 2: Performance Monitoring Table

**Typical Structure**:
```sql
CREATE TABLE performance_metrics (
  metric_id int(11) NOT NULL auto_increment,
  server_id int(11) NOT NULL,
  metric_name varchar(100),
  metric_value double,
  record_time datetime,
  PRIMARY KEY (metric_id),
  KEY idx_server_time (server_id, record_time)
);
```

**Key Implementation**:
```c
typedef struct {
    unsigned int metric_id;
    int server_id;
    char metric_name[101];
    double metric_value;
    char record_time[20];
} PerformanceMetrics;

int PerformanceMetrics_Add(DBConnectionManager* manager, const PerformanceMetrics* metric) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !metric) return -1;

    // Use NOW() if record_time is empty
    if (metric->record_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO performance_metrics (server_id, metric_name, metric_value, record_time) "
            "VALUES (%d, '%s', %.2f, '%s')",
            metric->server_id, metric->metric_name, metric->metric_value, metric->record_time);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO performance_metrics (server_id, metric_name, metric_value, record_time) "
            "VALUES (%d, '%s', %.2f, NOW())",
            metric->server_id, metric->metric_name, metric->metric_value);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

// Query by server and time range
int PerformanceMetrics_GetByServerAndTime(DBConnectionManager* manager,
                                           int server_id,
                                           const char* start_time,
                                           const char* end_time,
                                           PerformanceMetrics* metrics,
                                           int max_count,
                                           int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !metrics || !actual_count || !start_time || !end_time) return -1;

    snprintf(query, sizeof(query),
        "SELECT metric_id, server_id, metric_name, metric_value, record_time "
        "FROM performance_metrics "
        "WHERE server_id = %d AND record_time BETWEEN '%s' AND '%s' "
        "ORDER BY record_time DESC LIMIT %d",
        server_id, start_time, end_time, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&metrics[count], 0, sizeof(PerformanceMetrics));
        metrics[count].metric_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        metrics[count].server_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(metrics[count].metric_name, row[2], sizeof(metrics[count].metric_name) - 1);
        metrics[count].metric_value = row[3] ? atof(row[3]) : 0.0;
        if (row[4]) strncpy(metrics[count].record_time, row[4], sizeof(metrics[count].record_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: Double Type Formatting
```c
// ❌ Wrong - using %d for double
snprintf(query, sizeof(query),
    "INSERT INTO metrics (value) VALUES (%d)", metric_value);

// ✅ Correct - using %f or %.2f for double
snprintf(query, sizeof(query),
    "INSERT INTO metrics (value) VALUES (%.2f)", metric_value);
```

### Error 2: IP Address Field Size
```c
// ❌ Wrong - IPv6 requires more space
char ip_address[16];  // Only fits IPv4

// ✅ Correct - IPv6 needs up to 45 chars
char ip_address[46];  // VARCHAR(45) → 46 for '\0'
```

### Error 3: DATETIME Range Query
```c
// ❌ Wrong - missing quotes around datetime
snprintf(query, sizeof(query),
    "WHERE event_time BETWEEN %s AND %s", start, end);

// ✅ Correct - datetime must be quoted
snprintf(query, sizeof(query),
    "WHERE event_time BETWEEN '%s' AND '%s'", start, end);
```

### Error 4: Converting String to Double
```c
// Parse double from query result
metrics->metric_value = row[3] ? atof(row[3]) : 0.0;  // ✅ Use atof, not atoi
```

---

## Standard Development Process

### Complete Workflow (9 Steps)

```
Step 0: Find table structure → Determine database (d_taiwan_secu or d_technical_report)
Step 1: Create test table → Create in test_d_taiwan
Step 2: Create header file → security_monitoring/<database>/headers/<table>_orm.h
Step 3: Create source file → security_monitoring/<database>/sources/<table>_orm.c
Step 4: Create test file → security_monitoring/<database>/tests/test_<table>_orm.c
Step 5: Upload files → scp to server
Step 6: Deploy files → cp to compilation directory
Step 7: Compile test → gcc compile
Step 8: Run test → Verify functionality
Step 9: Update status → Mark in security_monitoring/ALL_DATABASES_TABLES.md
```

---

### Step 0: Find Table Structure

**Search in security monitoring databases**:
```bash
# Search d_taiwan_secu
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 d_taiwan_secu -e "SHOW TABLES" | grep -i <table_name>'

# Search d_technical_report
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 d_technical_report -e "SHOW TABLES" | grep -i <table_name>'

# Get table structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table_name>\\G"'
```

**Database Type**: Both use `DB_TYPE_TAIWAN = 1`

---

### Step 1: Create Test Table

```bash
# Both databases use test_d_taiwan
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_d_taiwan -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure from Step 0
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# Verify
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_d_taiwan -e "DESCRIBE <table>"'
```

---

### Step 2-4: Create Code Files

Create files in the appropriate database module directory:

**For d_taiwan_secu tables**:
```
security_monitoring/d_taiwan_secu/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

**For d_technical_report tables**:
```
security_monitoring/d_technical_report/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

### Step 5-9: Upload, Deploy, Compile, Test, Update

Same process as core_game, but adjust paths:

```bash
# Upload (example for d_taiwan_secu)
cd "C:\Users\waw\Desktop\local_orm"
scp "security_monitoring/d_taiwan_secu/headers/<table>_orm.h" vm131:/tmp/dnf_new_orm/headers/
scp "security_monitoring/d_taiwan_secu/sources/<table>_orm.c" vm131:/tmp/dnf_new_orm/sources/
scp "security_monitoring/d_taiwan_secu/tests/test_<table>_orm.c" vm131:/tmp/dnf_new_orm/tests/

# Deploy, compile, test - same as core_game
# ...

# Update security_monitoring/ALL_DATABASES_TABLES.md
```

---

## Quick Development Script

**Automation script** `dev_security_module.sh`:
```bash
#!/bin/bash
DATABASE=$1  # d_taiwan_secu or d_technical_report
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_security_module.sh <database> <table_name>"
    echo "Example: ./dev_security_module.sh d_taiwan_secu security_log"
    echo ""
    echo "Available databases:"
    echo "  - d_taiwan_secu (8 tables)"
    echo "  - d_technical_report (42 tables)"
    exit 1
fi

echo "=== Developing Security Monitoring Module: $DATABASE/$TABLE ==="

cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading files..."
scp "security_monitoring/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "security_monitoring/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "security_monitoring/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
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
echo "Next: Update security_monitoring/ALL_DATABASES_TABLES.md"
```

**Usage**:
```bash
chmod +x dev_security_module.sh
./dev_security_module.sh d_taiwan_secu security_log
./dev_security_module.sh d_technical_report performance_metrics
```

---

## Test File Template

```c
#include "security_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_add_and_get(DBConnectionManager* manager);
void test_query_operations(DBConnectionManager* manager);
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

    /* Security monitoring uses DB_TYPE_TAIWAN */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Security Monitoring ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_add_and_get(&manager);
    test_query_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM security_log WHERE m_id IN (999001, 999002)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
    DBQueryResult_Free(&result);
}
```

---

## Quick Reference

### Database Type
```c
DB_TYPE_TAIWAN = 1  // Both d_taiwan_secu and d_technical_report
```

### Test Database
```
test_d_taiwan  // For both d_taiwan_secu and d_technical_report tables
```

### Development Checklist
```
□ Determined correct database (d_taiwan_secu or d_technical_report)
□ Used DB_TYPE_TAIWAN for all operations
□ Test files use English-only comments
□ IP address fields use char[46] for IPv6 support
□ Double values use %.2f formatting
□ DATETIME queries have quotes
□ Time-range queries use BETWEEN
□ All results are freed
□ Test data cleaned up
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\security_monitoring`
- `d_taiwan_secu/` - Security monitoring (8 tables)
- `d_technical_report/` - Technical reports (42 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
- Test database: `test_d_taiwan`
- User: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Security Monitoring Specific Guide
