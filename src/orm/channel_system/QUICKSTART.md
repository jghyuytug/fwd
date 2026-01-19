# Channel System ORM Development Quick Guide

**2 Database Modules | 8 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Channel System Module Overview

### 2 Database Modules (8 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `d_channel/` | 7 | Channel management | `DB_TYPE_TAIWAN = 1` |
| `taiwan_login_play/` | 1 | Login play data | `DB_TYPE_LOGIN = 4` |

### Module Structure

```
<database>/
├── headers/    # ORM header files (.h)
├── sources/    # ORM source files (.c)
├── tests/      # Test files (test_*.c)
├── sql/        # SQL scripts
└── docs/       # Module documentation
```

**Example**: Working on `d_channel/channel_info` table
```
channel_system/d_channel/
├── headers/channel_info_orm.h
├── sources/channel_info_orm.c
├── tests/test_channel_info_orm.c
└── sql/channel_info.sql
```

---

## Channel Table Characteristics

Channel tables typically have:
- **Channel identification** (channel_id, server_id)
- **Status tracking** (is_online, player_count, max_players)
- **Server information** (ip_address, port)
- **Capacity management** (current_load, max_capacity)
- **Time tracking** (start_time, last_heartbeat)

---

## Development Templates

### Template 1: Channel Info Table

**Typical Structure**:
```sql
CREATE TABLE channel_info (
  channel_id int(11) NOT NULL,
  server_id int(11) NOT NULL,
  channel_name varchar(100),
  ip_address varchar(45),
  port int(11),
  max_players int(11) default 1000,
  current_players int(11) default 0,
  is_online enum('y','n') default 'n',
  last_heartbeat datetime,
  PRIMARY KEY (channel_id, server_id),
  KEY idx_online (is_online, current_players)
);
```

**Header File** `channel_info_orm.h`:
```c
#ifndef CHANNEL_INFO_ORM_H
#define CHANNEL_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int channel_id;
    int server_id;
    char channel_name[101];
    char ip_address[46];
    int port;
    int max_players;
    int current_players;
    char is_online;  // 'y' or 'n'
    char last_heartbeat[20];
} ChannelInfo;

/* CRUD Operations */
int ChannelInfo_Add(DBConnectionManager* manager, const ChannelInfo* channel);
int ChannelInfo_Get(DBConnectionManager* manager, int channel_id, int server_id, ChannelInfo* channel);
int ChannelInfo_Update(DBConnectionManager* manager, const ChannelInfo* channel);
int ChannelInfo_Delete(DBConnectionManager* manager, int channel_id, int server_id);

/* Business Operations */
int ChannelInfo_SetOnline(DBConnectionManager* manager, int channel_id, int server_id, char is_online);
int ChannelInfo_UpdatePlayerCount(DBConnectionManager* manager, int channel_id, int server_id, int count);
int ChannelInfo_UpdateHeartbeat(DBConnectionManager* manager, int channel_id, int server_id);
int ChannelInfo_GetOnlineChannels(DBConnectionManager* manager, ChannelInfo* channels,
                                   int max_count, int* actual_count);
int ChannelInfo_GetByServer(DBConnectionManager* manager, int server_id, ChannelInfo* channels,
                             int max_count, int* actual_count);

void ChannelInfo_PrintInfo(const ChannelInfo* channel);

#endif
```

**Key Implementation**:
```c
int ChannelInfo_UpdateHeartbeat(DBConnectionManager* manager, int channel_id, int server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE channel_info SET last_heartbeat = NOW() "
        "WHERE channel_id = %d AND server_id = %d",
        channel_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChannelInfo_GetOnlineChannels(DBConnectionManager* manager, ChannelInfo* channels,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !channels || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_id, server_id, channel_name, ip_address, port, "
        "max_players, current_players, is_online, last_heartbeat "
        "FROM channel_info "
        "WHERE is_online = 'y' "
        "ORDER BY current_players ASC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&channels[count], 0, sizeof(ChannelInfo));
        channels[count].channel_id = row[0] ? atoi(row[0]) : 0;
        channels[count].server_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(channels[count].channel_name, row[2], sizeof(channels[count].channel_name) - 1);
        if (row[3]) strncpy(channels[count].ip_address, row[3], sizeof(channels[count].ip_address) - 1);
        channels[count].port = row[4] ? atoi(row[4]) : 0;
        channels[count].max_players = row[5] ? atoi(row[5]) : 1000;
        channels[count].current_players = row[6] ? atoi(row[6]) : 0;
        channels[count].is_online = row[7] ? row[7][0] : 'n';
        if (row[8]) strncpy(channels[count].last_heartbeat, row[8], sizeof(channels[count].last_heartbeat) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

### Template 2: Login Play Data Table

**Typical Structure**:
```sql
CREATE TABLE login_play_data (
  m_id int(11) NOT NULL,
  server_id int(11) NOT NULL,
  channel_id int(11),
  login_time datetime,
  logout_time datetime,
  play_duration int(11),
  PRIMARY KEY (m_id, server_id),
  KEY idx_login_time (login_time)
);
```

**Key Implementation**:
```c
typedef struct {
    int m_id;
    int server_id;
    int channel_id;
    char login_time[20];
    char logout_time[20];
    int play_duration;  // in seconds
} LoginPlayData;

int LoginPlayData_RecordLogin(DBConnectionManager* manager, int m_id, int server_id, int channel_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO login_play_data (m_id, server_id, channel_id, login_time) "
        "VALUES (%d, %d, %d, NOW()) "
        "ON DUPLICATE KEY UPDATE channel_id = %d, login_time = NOW(), logout_time = NULL, play_duration = 0",
        m_id, server_id, channel_id, channel_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginPlayData_RecordLogout(DBConnectionManager* manager, int m_id, int server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE login_play_data "
        "SET logout_time = NOW(), play_duration = TIMESTAMPDIFF(SECOND, login_time, NOW()) "
        "WHERE m_id = %d AND server_id = %d",
        m_id, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: Composite Primary Key
```c
// ❌ Wrong - only using channel_id
ChannelInfo_Get(manager, channel_id, &channel);

// ✅ Correct - using both channel_id and server_id
ChannelInfo_Get(manager, channel_id, server_id, &channel);
```

### Error 2: Database Type Selection
```c
// d_channel tables
DB_TYPE_TAIWAN = 1

// taiwan_login_play tables
DB_TYPE_LOGIN = 4

// ❌ Wrong
DBConnectionManager_Connect(&manager, DB_TYPE_TAIWAN);  // For login_play_data

// ✅ Correct
DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN);   // For login_play_data
```

### Error 3: Player Count Increment
```c
// ❌ Wrong - race condition possible
current = GetCurrentPlayers();
UpdatePlayers(current + 1);

// ✅ Correct - atomic increment
UPDATE channel_info SET current_players = current_players + 1
WHERE channel_id = ? AND server_id = ?;
```

---

## Standard Development Process

### Step 0: Find Table Structure

```bash
# Search d_channel
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 d_channel -e "SHOW TABLES" | grep -i <table>'

# Search taiwan_login_play
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_login_play -e "SHOW TABLES" | grep -i <table>'

# Get structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table>\\G"'
```

### Step 1: Create Test Table

```bash
# For d_channel tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_d_channel -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# For taiwan_login_play tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_login_play -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'
```

### Step 2-4: Create Code Files

**For d_channel tables**:
```
channel_system/d_channel/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

**For taiwan_login_play tables**:
```
channel_system/taiwan_login_play/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

## Quick Development Script

```bash
#!/bin/bash
DATABASE=$1  # d_channel or taiwan_login_play
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_channel_module.sh <database> <table_name>"
    echo "Example: ./dev_channel_module.sh d_channel channel_info"
    echo ""
    echo "Available databases:"
    echo "  - d_channel (7 tables) - DB_TYPE_TAIWAN"
    echo "  - taiwan_login_play (1 table) - DB_TYPE_LOGIN"
    exit 1
fi

echo "=== Developing Channel System: $DATABASE/$TABLE ==="
cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading..."
scp "channel_system/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "channel_system/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "channel_system/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
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

echo "[5/5] Complete! Update channel_system/ALL_DATABASES_TABLES.md"
```

---

## Quick Reference

### Database Types
```c
DB_TYPE_TAIWAN = 1  // d_channel
DB_TYPE_LOGIN  = 4  // taiwan_login_play
```

### Test Databases
```
test_d_channel
test_taiwan_login_play
```

### Development Checklist
```
□ Selected correct DB_TYPE (TAIWAN for d_channel, LOGIN for taiwan_login_play)
□ Composite keys include all fields (channel_id, server_id)
□ IP address fields use char[46] for IPv6
□ NOW() used for heartbeat updates
□ Atomic operations for player count updates
□ Test files use English-only comments
□ All results are freed
□ Test data cleaned up
```

---

## Best Practices

### 1. Heartbeat Management
```c
// ✅ Good - Update heartbeat periodically
UPDATE channel_info SET last_heartbeat = NOW()
WHERE channel_id = ? AND server_id = ?;

// Check stale channels
SELECT * FROM channel_info
WHERE is_online = 'y'
  AND last_heartbeat < DATE_SUB(NOW(), INTERVAL 5 MINUTE);
```

### 2. Player Count Management
```c
// ✅ Good - Atomic increment/decrement
UPDATE channel_info SET current_players = current_players + 1;
UPDATE channel_info SET current_players = current_players - 1;

// Prevent negative counts
UPDATE channel_info SET current_players = GREATEST(0, current_players - 1);
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\channel_system`
- `d_channel/` - Channel management (7 tables)
- `taiwan_login_play/` - Login play data (1 table)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
**Test Databases**: `test_d_channel`, `test_taiwan_login_play`
**User**: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Channel System Specific Guide
