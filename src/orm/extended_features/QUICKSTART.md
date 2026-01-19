# Extended Features ORM Development Quick Guide

**5 Database Modules | 85 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Extended Features Module Overview

### 5 Database Modules (85 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `taiwan_cain_2nd/` | 32 | Mail & messaging system | `DB_TYPE_CAIN = 2` |
| `taiwan_mng_manager/` | 21 | GM management tools | `DB_TYPE_TAIWAN = 1` |
| `taiwan_cain_web/` | 17 | Web data interface | `DB_TYPE_CAIN = 2` |
| `qq_bot/` | 9 | QQ bot integration | `DB_TYPE_TAIWAN = 1` |
| `gm_gui/` | 6 | GM GUI tools | `DB_TYPE_TAIWAN = 1` |

### Module Structure

```
<database>/
├── headers/    # ORM header files (.h)
├── sources/    # ORM source files (.c)
├── tests/      # Test files (test_*.c)
├── sql/        # SQL scripts
└── docs/       # Module documentation
```

---

## Development Templates

### Template 1: Mail/Message Table (taiwan_cain_2nd)

**Typical Structure**:
```sql
CREATE TABLE mail_message (
  mail_id bigint(20) NOT NULL auto_increment,
  sender_charac_no int(11) NOT NULL,
  receiver_charac_no int(11) NOT NULL,
  subject varchar(200),
  message text,
  send_time datetime,
  read_flag enum('y','n') default 'n',
  delete_flag enum('y','n') default 'n',
  PRIMARY KEY (mail_id),
  KEY idx_receiver (receiver_charac_no, read_flag)
);
```

**Header File** `mail_message_orm.h`:
```c
#ifndef MAIL_MESSAGE_ORM_H
#define MAIL_MESSAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long mail_id;
    int sender_charac_no;
    int receiver_charac_no;
    char subject[201];
    char message[2048];  // TEXT field
    char send_time[20];
    char read_flag;   // 'y' or 'n'
    char delete_flag; // 'y' or 'n'
} MailMessage;

/* CRUD Operations */
int MailMessage_Send(DBConnectionManager* manager, const MailMessage* mail);
int MailMessage_Get(DBConnectionManager* manager, unsigned long long mail_id, MailMessage* mail);
int MailMessage_MarkAsRead(DBConnectionManager* manager, unsigned long long mail_id);
int MailMessage_Delete(DBConnectionManager* manager, unsigned long long mail_id);

/* Query Operations */
int MailMessage_GetInbox(DBConnectionManager* manager, int receiver_charac_no,
                         MailMessage* mails, int max_count, int* actual_count);
int MailMessage_GetUnreadCount(DBConnectionManager* manager, int receiver_charac_no, int* count);

void MailMessage_PrintInfo(const MailMessage* mail);

#endif
```

---

### Template 2: GM Tool Table (taiwan_mng_manager)

**Typical Structure**:
```sql
CREATE TABLE gm_action_log (
  log_id int(11) NOT NULL auto_increment,
  gm_account varchar(50),
  action_type varchar(100),
  target_m_id int(11),
  action_detail text,
  action_time datetime,
  PRIMARY KEY (log_id),
  KEY idx_gm (gm_account, action_time)
);
```

**Header File** `gm_action_log_orm.h`:
```c
#ifndef GM_ACTION_LOG_ORM_H
#define GM_ACTION_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int log_id;
    char gm_account[51];
    char action_type[101];
    int target_m_id;
    char action_detail[2048];
    char action_time[20];
} GmActionLog;

/* Operations */
int GmActionLog_Add(DBConnectionManager* manager, const GmActionLog* log);
int GmActionLog_GetByGM(DBConnectionManager* manager, const char* gm_account,
                         GmActionLog* logs, int max_count, int* actual_count);
int GmActionLog_GetByTimeRange(DBConnectionManager* manager,
                                 const char* start_time, const char* end_time,
                                 GmActionLog* logs, int max_count, int* actual_count);

void GmActionLog_PrintInfo(const GmActionLog* log);

#endif
```

---

## Common Errors

### Error 1: TEXT Field Buffer Size
```c
// ❌ Wrong - TEXT needs large buffer
char message[256];

// ✅ Correct - Allocate adequate size
char message[2048];  // Or larger based on actual usage
```

### Error 2: Database Type Selection
```c
// ❌ Wrong - using wrong DB_TYPE
DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN);  // For taiwan_cain_2nd

// ✅ Correct - check database name
// taiwan_cain_2nd → DB_TYPE_CAIN = 2
// taiwan_mng_manager → DB_TYPE_TAIWAN = 1
// taiwan_cain_web → DB_TYPE_CAIN = 2
// qq_bot → DB_TYPE_TAIWAN = 1
// gm_gui → DB_TYPE_TAIWAN = 1
```

### Error 3: Mail Status Flags
```c
// Use separate flags for read and delete
char read_flag;   // 'y' = read, 'n' = unread
char delete_flag; // 'y' = deleted, 'n' = active

// Query unread mails
WHERE read_flag = 'n' AND delete_flag = 'n'
```

---

## Standard Development Process

### Step 0: Find Table Structure

```bash
# Search appropriate database
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_2nd -e "SHOW TABLES" | grep -i <table>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_mng_manager -e "SHOW TABLES" | grep -i <table>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_web -e "SHOW TABLES" | grep -i <table>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 qq_bot -e "SHOW TABLES" | grep -i <table>'
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 gm_gui -e "SHOW TABLES" | grep -i <table>'

# Get structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table>\\G"'
```

### Step 1: Create Test Table

```bash
# Use appropriate test database
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_<database> -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'
```

### Step 2-4: Create Code Files

Create in appropriate database module directory:
```
extended_features/<database>/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

## Quick Development Script

```bash
#!/bin/bash
DATABASE=$1  # taiwan_cain_2nd, taiwan_mng_manager, taiwan_cain_web, qq_bot, gm_gui
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_extended_module.sh <database> <table_name>"
    echo ""
    echo "Available databases:"
    echo "  - taiwan_cain_2nd (32 tables) - DB_TYPE_CAIN"
    echo "  - taiwan_mng_manager (21 tables) - DB_TYPE_TAIWAN"
    echo "  - taiwan_cain_web (17 tables) - DB_TYPE_CAIN"
    echo "  - qq_bot (9 tables) - DB_TYPE_TAIWAN"
    echo "  - gm_gui (6 tables) - DB_TYPE_TAIWAN"
    exit 1
fi

echo "=== Developing Extended Features: $DATABASE/$TABLE ==="
cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading..."
scp "extended_features/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "extended_features/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "extended_features/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
wait
echo "✓ Upload complete"

# Deploy
echo "[2/5] Deploying..."
ssh vm131 "cp /tmp/dnf_new_orm/sources/${TABLE}_orm.c /tmp/dnf_new_orm/src/database/ && \
           cp /tmp/dnf_new_orm/headers/${TABLE}_orm.h /tmp/dnf_new_orm/include/database/headers/"
echo "✓ Deploy complete"

# Compile
echo "[3/5] Compiling..."
ssh vm131 "cd /tmp/dnf_new_orm/tests && gcc -m32 -g -Wall -Wextra -std=c99 \
    -I../include -I../include/database/headers -I../include/common -I../include/common/headers \
    -o test_${TABLE}_orm test_${TABLE}_orm.c \
    ../src/database/db_connection_manager.c ../src/database/${TABLE}_orm.c \
    -L/usr/lib/mysql -lmysqlclient -lpthread 2>&1"

[ $? -eq 0 ] && echo "✓ Compile successful" || { echo "✗ Compile failed"; exit 1; }

# Test
echo "[4/5] Testing..."
ssh vm131 "/tmp/dnf_new_orm/tests/test_${TABLE}_orm /tmp/dnf_new_orm/config/database.cfg 2>&1 | grep -v POOL_DEBUG"

[ $? -eq 0 ] && echo "✓ Test passed" || { echo "✗ Test failed"; exit 1; }

echo "[5/5] Complete! Update extended_features/ALL_DATABASES_TABLES.md"
```

---

## Quick Reference

### Database Types

```c
// taiwan_cain_2nd
DB_TYPE_CAIN = 2

// taiwan_mng_manager
DB_TYPE_TAIWAN = 1

// taiwan_cain_web
DB_TYPE_CAIN = 2

// qq_bot
DB_TYPE_TAIWAN = 1

// gm_gui
DB_TYPE_TAIWAN = 1
```

### Test Databases
```
test_taiwan_cain_2nd
test_taiwan_mng_manager
test_taiwan_cain_web
test_qq_bot
test_gm_gui
```

### Development Checklist
```
□ Selected correct DB_TYPE based on database name
□ TEXT fields have large buffers (2048+)
□ BIGINT mail IDs use unsigned long long and atoll()
□ Multiple ENUM flags handled separately
□ Test files use English-only comments
□ All results are freed
□ Test data cleaned up
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\extended_features`
- `taiwan_cain_2nd/` - Mail system (32 tables)
- `taiwan_mng_manager/` - GM tools (21 tables)
- `taiwan_cain_web/` - Web interface (17 tables)
- `qq_bot/` - QQ bot (9 tables)
- `gm_gui/` - GM GUI (6 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
**User**: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Extended Features Specific Guide
