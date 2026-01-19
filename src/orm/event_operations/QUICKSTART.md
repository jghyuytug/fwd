# Event Operations ORM Development Quick Guide

**2 Database Modules | 64 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Encountered errors? → [Common Errors](#common-errors)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Event Operations Module Overview

### 2 Database Modules (64 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `taiwan_game_event/` | 51 | Game events & promotions | `DB_TYPE_CAIN = 2` |
| `taiwan_se_event/` | 13 | Special events | `DB_TYPE_CAIN = 2` |

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

**Example**: Working on `taiwan_game_event/event_info` table
```
event_operations/taiwan_game_event/
├── headers/event_info_orm.h
├── sources/event_info_orm.c
├── tests/test_event_info_orm.c
└── sql/event_info.sql
```

---

## Event Table Characteristics

Event tables typically have:
- **Event identification** (event_id, event_code)
- **Time constraints** (start_date, end_date)
- **Status flags** (is_active, is_deleted)
- **Participation tracking** (m_id, charac_no, entry_count)
- **Reward information** (reward_type, reward_amount)
- **Date/time fields** for event periods

---

## Development Templates

### Template 1: Event Configuration Table

**Typical Structure**:
```sql
CREATE TABLE event_info (
  event_id int(11) NOT NULL auto_increment,
  event_code varchar(50) NOT NULL,
  event_name varchar(200),
  start_date datetime,
  end_date datetime,
  is_active enum('y','n') default 'n',
  max_participants int(11) default 0,
  PRIMARY KEY (event_id),
  UNIQUE KEY (event_code)
);
```

**Header File** `event_info_orm.h`:
```c
#ifndef EVENT_INFO_ORM_H
#define EVENT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int event_id;
    char event_code[51];
    char event_name[201];
    char start_date[20];
    char end_date[20];
    char is_active;  // 'y' or 'n'
    int max_participants;
} EventInfo;

/* CRUD Operations */
int EventInfo_Add(DBConnectionManager* manager, const EventInfo* event);
int EventInfo_Get(DBConnectionManager* manager, unsigned int event_id, EventInfo* event);
int EventInfo_GetByCode(DBConnectionManager* manager, const char* event_code, EventInfo* event);
int EventInfo_Update(DBConnectionManager* manager, const EventInfo* event);
int EventInfo_Delete(DBConnectionManager* manager, unsigned int event_id);

/* Business Operations */
int EventInfo_GetActiveEvents(DBConnectionManager* manager, EventInfo* events,
                               int max_count, int* actual_count);
int EventInfo_GetEventsByDateRange(DBConnectionManager* manager,
                                    const char* start, const char* end,
                                    EventInfo* events, int max_count, int* actual_count);
int EventInfo_SetActive(DBConnectionManager* manager, unsigned int event_id, char is_active);

/* Utilities */
void EventInfo_PrintInfo(const EventInfo* event);

#endif
```

**Key Implementation**:
```c
int EventInfo_GetActiveEvents(DBConnectionManager* manager, EventInfo* events,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !events || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, event_code, event_name, start_date, end_date, is_active, max_participants "
        "FROM event_info "
        "WHERE is_active = 'y' AND start_date <= NOW() AND end_date >= NOW() "
        "ORDER BY start_date DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&events[count], 0, sizeof(EventInfo));
        events[count].event_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(events[count].event_code, row[1], sizeof(events[count].event_code) - 1);
        if (row[2]) strncpy(events[count].event_name, row[2], sizeof(events[count].event_name) - 1);
        if (row[3]) strncpy(events[count].start_date, row[3], sizeof(events[count].start_date) - 1);
        if (row[4]) strncpy(events[count].end_date, row[4], sizeof(events[count].end_date) - 1);
        events[count].is_active = row[5] ? row[5][0] : 'n';
        events[count].max_participants = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

### Template 2: Event Participation Table

**Typical Structure**:
```sql
CREATE TABLE event_participation (
  entry_id int(11) NOT NULL auto_increment,
  event_code varchar(50) NOT NULL,
  m_id int(11) NOT NULL,
  charac_no int(11) NOT NULL,
  entry_count int(11) default 1,
  last_entry_time datetime,
  reward_received enum('y','n') default 'n',
  PRIMARY KEY (entry_id),
  UNIQUE KEY (event_code, m_id, charac_no),
  KEY idx_event (event_code),
  KEY idx_member (m_id)
);
```

**Header File** `event_participation_orm.h`:
```c
#ifndef EVENT_PARTICIPATION_ORM_H
#define EVENT_PARTICIPATION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int entry_id;
    char event_code[51];
    int m_id;
    int charac_no;
    int entry_count;
    char last_entry_time[20];
    char reward_received;  // 'y' or 'n'
} EventParticipation;

/* CRUD Operations */
int EventParticipation_Add(DBConnectionManager* manager, const EventParticipation* entry);
int EventParticipation_Get(DBConnectionManager* manager, unsigned int entry_id,
                            EventParticipation* entry);
int EventParticipation_GetByEventAndCharac(DBConnectionManager* manager,
                                            const char* event_code, int charac_no,
                                            EventParticipation* entry);
int EventParticipation_Update(DBConnectionManager* manager, const EventParticipation* entry);

/* Business Operations */
int EventParticipation_IncrementEntryCount(DBConnectionManager* manager,
                                            const char* event_code, int charac_no);
int EventParticipation_SetRewardReceived(DBConnectionManager* manager,
                                          const char* event_code, int charac_no);
int EventParticipation_GetParticipantsByEvent(DBConnectionManager* manager,
                                               const char* event_code,
                                               EventParticipation* entries,
                                               int max_count, int* actual_count);
int EventParticipation_GetMemberEvents(DBConnectionManager* manager, int m_id,
                                        EventParticipation* entries,
                                        int max_count, int* actual_count);

/* Utilities */
void EventParticipation_PrintInfo(const EventParticipation* entry);

#endif
```

**Key Implementation**:
```c
int EventParticipation_IncrementEntryCount(DBConnectionManager* manager,
                                            const char* event_code, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !event_code) return -1;

    // Use ON DUPLICATE KEY UPDATE to increment or insert
    snprintf(query, sizeof(query),
        "INSERT INTO event_participation (event_code, m_id, charac_no, entry_count, last_entry_time) "
        "SELECT '%s', m_id, %d, 1, NOW() FROM charac_info WHERE charac_no = %d "
        "ON DUPLICATE KEY UPDATE entry_count = entry_count + 1, last_entry_time = NOW()",
        event_code, charac_no, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: Event Date Comparison
```c
// ❌ Wrong - comparing datetime as string without quotes
snprintf(query, sizeof(query),
    "WHERE start_date <= %s AND end_date >= %s", start, end);

// ✅ Correct - quote datetime values
snprintf(query, sizeof(query),
    "WHERE start_date <= '%s' AND end_date >= '%s'", start, end);

// ✅ Best - use NOW() for current time comparison
snprintf(query, sizeof(query),
    "WHERE start_date <= NOW() AND end_date >= NOW()");
```

### Error 2: ENUM Status Flag Handling
```c
// ❌ Wrong - comparing char to string
if (event->is_active == "y")

// ✅ Correct - comparing char to char
if (event->is_active == 'y')
```

### Error 3: Unique Constraint Violation
```c
// ❌ Wrong - insert may fail on duplicate
EventParticipation_Add(manager, &entry);  // Fails if already exists

// ✅ Correct - use ON DUPLICATE KEY UPDATE
snprintf(query, sizeof(query),
    "INSERT INTO event_participation (...) VALUES (...) "
    "ON DUPLICATE KEY UPDATE entry_count = entry_count + 1");
```

### Error 4: Event Code Validation
```c
// ❌ Risky - no validation of event_code format
strcpy(event->event_code, user_input);

// ✅ Better - validate event_code format
if (strlen(user_input) > 0 && strlen(user_input) < 50) {
    strncpy(event->event_code, user_input, sizeof(event->event_code) - 1);
} else {
    return -1;  // Invalid event code
}
```

---

## Standard Development Process

### Complete Workflow (9 Steps)

```
Step 0: Find table structure → Determine database (taiwan_game_event or taiwan_se_event)
Step 1: Create test table → Create in test_taiwan_game_event or test_taiwan_se_event
Step 2: Create header file → event_operations/<database>/headers/<table>_orm.h
Step 3: Create source file → event_operations/<database>/sources/<table>_orm.c
Step 4: Create test file → event_operations/<database>/tests/test_<table>_orm.c
Step 5: Upload files → scp to server
Step 6: Deploy files → cp to compilation directory
Step 7: Compile test → gcc compile
Step 8: Run test → Verify functionality
Step 9: Update status → Mark in event_operations/ALL_DATABASES_TABLES.md
```

---

### Step 0: Find Table Structure

**Search in event databases**:
```bash
# Search taiwan_game_event
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_game_event -e "SHOW TABLES" | grep -i <table_name>'

# Search taiwan_se_event
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_se_event -e "SHOW TABLES" | grep -i <table_name>'

# Get table structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table_name>\\G"'
```

**Database Type**: Both use `DB_TYPE_CAIN = 2`

---

### Step 1: Create Test Table

```bash
# For taiwan_game_event tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_game_event -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure from Step 0
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# For taiwan_se_event tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_se_event -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure from Step 0
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# Verify
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_<database> -e "DESCRIBE <table>"'
```

---

### Step 2-4: Create Code Files

**For taiwan_game_event tables**:
```
event_operations/taiwan_game_event/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

**For taiwan_se_event tables**:
```
event_operations/taiwan_se_event/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

## Quick Development Script

**Automation script** `dev_event_module.sh`:
```bash
#!/bin/bash
DATABASE=$1  # taiwan_game_event or taiwan_se_event
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_event_module.sh <database> <table_name>"
    echo "Example: ./dev_event_module.sh taiwan_game_event event_info"
    echo ""
    echo "Available databases:"
    echo "  - taiwan_game_event (51 tables)"
    echo "  - taiwan_se_event (13 tables)"
    exit 1
fi

echo "=== Developing Event Operations Module: $DATABASE/$TABLE ==="

cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading files..."
scp "event_operations/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "event_operations/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "event_operations/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
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
echo "Next: Update event_operations/ALL_DATABASES_TABLES.md"
```

**Usage**:
```bash
chmod +x dev_event_module.sh
./dev_event_module.sh taiwan_game_event event_info
./dev_event_module.sh taiwan_se_event special_event
```

---

## Test File Template

```c
#include "event_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_event_crud(DBConnectionManager* manager);
void test_active_events(DBConnectionManager* manager);
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

    /* Event operations use DB_TYPE_CAIN */
    if (DBConnectionManager_Connect(&manager, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== Event Operations ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_event_crud(&manager);
    test_active_events(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM event_info WHERE event_code LIKE 'TEST_%'");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);
    DBQueryResult_Free(&result);
}
```

---

## Quick Reference

### Database Type
```c
DB_TYPE_CAIN = 2  // Both taiwan_game_event and taiwan_se_event
```

### Test Databases
```
test_taiwan_game_event  // For taiwan_game_event tables
test_taiwan_se_event    // For taiwan_se_event tables
```

### Development Checklist
```
□ Determined correct database (taiwan_game_event or taiwan_se_event)
□ Used DB_TYPE_CAIN for all operations
□ ENUM status fields use char type
□ DATETIME comparisons have quotes
□ NOW() used for current time checks
□ ON DUPLICATE KEY UPDATE for participation tracking
□ Event code validation implemented
□ Test files use English-only comments
□ All results are freed
□ Test data cleaned up
```

---

## Best Practices for Event Tables

### 1. Active Event Queries
```c
// ✅ Good - Check both active flag and date range
SELECT * FROM event_info
WHERE is_active = 'y'
  AND start_date <= NOW()
  AND end_date >= NOW();
```

### 2. Participation Tracking
```c
// ✅ Good - Use UPSERT pattern for participation
INSERT INTO event_participation (event_code, charac_no, entry_count)
VALUES ('EVENT001', 12345, 1)
ON DUPLICATE KEY UPDATE
  entry_count = entry_count + 1,
  last_entry_time = NOW();
```

### 3. Event Period Validation
```c
// ✅ Good - Validate event dates before insert
if (strcmp(event->start_date, event->end_date) >= 0) {
    printf("ERROR: start_date must be before end_date\n");
    return -1;
}
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\event_operations`
- `taiwan_game_event/` - Game events (51 tables)
- `taiwan_se_event/` - Special events (13 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
- Test databases: `test_taiwan_game_event`, `test_taiwan_se_event`
- User: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Event Operations Specific Guide
