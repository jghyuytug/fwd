# Web Community ORM Development Quick Guide

**2 Database Modules | 476 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Encountered errors? → [Common Errors](#common-errors)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Web Community Module Overview

### 2 Database Modules (476 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `taiwan_main_web/` | 364 | Official website main site | `DB_TYPE_TAIWAN = 1` |
| `tw/` | 112 | d_taiwan backup/replica | `DB_TYPE_TAIWAN = 1` |

### Module Structure

```
<database>/
├── headers/    # ORM header files (.h)
├── sources/    # ORM source files (.c)
├── tests/      # Test files (test_*.c)
├── sql/        # SQL scripts
└── docs/       # Module documentation
```

**Example**: Working on `taiwan_main_web/web_user` table
```
web_community/taiwan_main_web/
├── headers/web_user_orm.h
├── sources/web_user_orm.c
├── tests/test_web_user_orm.c
└── sql/web_user.sql
```

---

## Web Community Table Characteristics

Web community tables typically have:
- **User management** (user_id, username, email, password_hash)
- **Content management** (post_id, title, content, author_id)
- **Forum/BBS features** (board_id, thread_id, reply_count, view_count)
- **Social features** (friend_id, follow_id, like_count, comment_count)
- **File attachments** (file_id, file_name, file_path, file_size)
- **Timestamps** (created_at, updated_at, last_login)

---

## Development Templates

### Template 1: Web User Table

**Typical Structure**:
```sql
CREATE TABLE web_user (
  user_id int(11) NOT NULL auto_increment,
  username varchar(50) NOT NULL,
  email varchar(100),
  password_hash varchar(255),
  nickname varchar(100),
  avatar_url varchar(255),
  register_date datetime,
  last_login datetime,
  is_active enum('y','n') default 'y',
  PRIMARY KEY (user_id),
  UNIQUE KEY (username),
  UNIQUE KEY (email),
  KEY idx_last_login (last_login)
);
```

**Header File** `web_user_orm.h`:
```c
#ifndef WEB_USER_ORM_H
#define WEB_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int user_id;
    char username[51];
    char email[101];
    char password_hash[256];
    char nickname[101];
    char avatar_url[256];
    char register_date[20];
    char last_login[20];
    char is_active;  // 'y' or 'n'
} WebUser;

/* CRUD Operations */
int WebUser_Add(DBConnectionManager* manager, const WebUser* user);
int WebUser_Get(DBConnectionManager* manager, unsigned int user_id, WebUser* user);
int WebUser_GetByUsername(DBConnectionManager* manager, const char* username, WebUser* user);
int WebUser_GetByEmail(DBConnectionManager* manager, const char* email, WebUser* user);
int WebUser_Update(DBConnectionManager* manager, const WebUser* user);
int WebUser_Delete(DBConnectionManager* manager, unsigned int user_id);

/* Business Operations */
int WebUser_UpdateLastLogin(DBConnectionManager* manager, unsigned int user_id);
int WebUser_SetActive(DBConnectionManager* manager, unsigned int user_id, char is_active);
int WebUser_GetRecentUsers(DBConnectionManager* manager, int days, WebUser* users,
                            int max_count, int* actual_count);

void WebUser_PrintInfo(const WebUser* user);

#endif
```

**Key Implementation**:
```c
int WebUser_UpdateLastLogin(DBConnectionManager* manager, unsigned int user_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE web_user SET last_login = NOW() WHERE user_id = %u",
        user_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
```

---

### Template 2: Forum Post Table

**Typical Structure**:
```sql
CREATE TABLE forum_post (
  post_id bigint(20) NOT NULL auto_increment,
  board_id int(11) NOT NULL,
  thread_id bigint(20),
  author_id int(11) NOT NULL,
  title varchar(200),
  content text,
  view_count int(11) default 0,
  reply_count int(11) default 0,
  is_pinned enum('y','n') default 'n',
  is_locked enum('y','n') default 'n',
  created_at datetime,
  updated_at datetime,
  PRIMARY KEY (post_id),
  KEY idx_board (board_id, created_at),
  KEY idx_author (author_id)
);
```

**Key Implementation**:
```c
typedef struct {
    unsigned long long post_id;
    int board_id;
    unsigned long long thread_id;
    int author_id;
    char title[201];
    char content[4096];  // TEXT field
    int view_count;
    int reply_count;
    char is_pinned;  // 'y' or 'n'
    char is_locked;  // 'y' or 'n'
    char created_at[20];
    char updated_at[20];
} ForumPost;

int ForumPost_IncrementViewCount(DBConnectionManager* manager, unsigned long long post_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE forum_post SET view_count = view_count + 1 WHERE post_id = %llu",
        post_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ForumPost_GetByBoard(DBConnectionManager* manager, int board_id,
                          ForumPost* posts, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !posts || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT post_id, board_id, thread_id, author_id, title, content, "
        "view_count, reply_count, is_pinned, is_locked, created_at, updated_at "
        "FROM forum_post "
        "WHERE board_id = %d "
        "ORDER BY is_pinned DESC, created_at DESC LIMIT %d",
        board_id, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&posts[count], 0, sizeof(ForumPost));
        posts[count].post_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        posts[count].board_id = row[1] ? atoi(row[1]) : 0;
        posts[count].thread_id = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        posts[count].author_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(posts[count].title, row[4], sizeof(posts[count].title) - 1);
        if (row[5]) strncpy(posts[count].content, row[5], sizeof(posts[count].content) - 1);
        posts[count].view_count = row[6] ? atoi(row[6]) : 0;
        posts[count].reply_count = row[7] ? atoi(row[7]) : 0;
        posts[count].is_pinned = row[8] ? row[8][0] : 'n';
        posts[count].is_locked = row[9] ? row[9][0] : 'n';
        if (row[10]) strncpy(posts[count].created_at, row[10], sizeof(posts[count].created_at) - 1);
        if (row[11]) strncpy(posts[count].updated_at, row[11], sizeof(posts[count].updated_at) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: TEXT Content Buffer Size
```c
// ❌ Wrong - TEXT needs large buffer
char content[512];

// ✅ Correct - Allocate adequate size for forum posts
char content[4096];  // Or larger based on actual usage

// For very large content, consider dynamic allocation
char* content = malloc(8192);
```

### Error 2: BIGINT for Post IDs
```c
// ❌ Wrong - using int for bigint(20)
int post_id = atoi(row[0]);

// ✅ Correct - use unsigned long long and atoll
unsigned long long post_id = atoll(row[0]);
```

### Error 3: Password Hash Storage
```c
// ❌ Wrong - storing plain text password
strcpy(user->password, plain_password);

// ✅ Correct - store password hash (255+ chars for bcrypt/etc)
char password_hash[256];
// In production, use proper password hashing (bcrypt, argon2, etc)
```

### Error 4: Email Validation
```c
// ❌ Risky - no email format validation
strcpy(user->email, user_input);

// ✅ Better - validate email format
if (strlen(user_input) > 0 && strlen(user_input) < 100 && strchr(user_input, '@')) {
    strncpy(user->email, user_input, sizeof(user->email) - 1);
} else {
    return -1;  // Invalid email
}
```

### Error 5: SQL Injection in Text Content
```c
// ❌ Dangerous - direct insertion of user content
snprintf(query, sizeof(query),
    "INSERT INTO forum_post (content) VALUES ('%s')", user_content);

// ✅ Critical - MUST escape user content
// In production: use mysql_real_escape_string() or prepared statements
// For development: ensure test data doesn't contain single quotes
```

---

## Standard Development Process

### Step 0: Find Table Structure

```bash
# Search taiwan_main_web (364 tables)
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_main_web -e "SHOW TABLES" | grep -i <table>'

# Search tw (112 tables - d_taiwan backup)
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 tw -e "SHOW TABLES" | grep -i <table>'

# Get structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table>\\G"'
```

### Step 1: Create Test Table

```bash
# For taiwan_main_web tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_taiwan_main_web -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'

# For tw tables
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_tw -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'
```

### Step 2-4: Create Code Files

**For taiwan_main_web tables**:
```
web_community/taiwan_main_web/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

**For tw tables**:
```
web_community/tw/
├── headers/<table>_orm.h
├── sources/<table>_orm.c
└── tests/test_<table>_orm.c
```

---

## Quick Development Script

```bash
#!/bin/bash
DATABASE=$1  # taiwan_main_web or tw
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_web_module.sh <database> <table_name>"
    echo "Example: ./dev_web_module.sh taiwan_main_web web_user"
    echo ""
    echo "Available databases:"
    echo "  - taiwan_main_web (364 tables) - Official website"
    echo "  - tw (112 tables) - d_taiwan backup"
    exit 1
fi

echo "=== Developing Web Community: $DATABASE/$TABLE ==="
cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading..."
scp "web_community/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "web_community/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "web_community/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
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

echo "[5/5] Complete! Update web_community/ALL_DATABASES_TABLES.md"
```

**Usage**:
```bash
chmod +x dev_web_module.sh
./dev_web_module.sh taiwan_main_web web_user
./dev_web_module.sh tw member_backup
```

---

## Quick Reference

### Database Type
```c
DB_TYPE_TAIWAN = 1  // Both taiwan_main_web and tw
```

### Test Databases
```
test_taiwan_main_web
test_tw
```

### Development Checklist
```
□ Used DB_TYPE_TAIWAN for all operations
□ BIGINT post/content IDs use unsigned long long and atoll()
□ TEXT content fields have large buffers (4096+)
□ Password hash fields are 256 chars
□ Email fields validated and have UNIQUE constraint
□ User content properly escaped (critical for security)
□ Atomic operations for view/reply count increments
□ Test files use English-only comments
□ All results are freed
□ Test data cleaned up
```

---

## Security Considerations

### CRITICAL: SQL Injection Prevention

**Web community tables handle user-generated content and are HIGH RISK for SQL injection.**

```c
// ❌ DANGEROUS - Never do this in production
snprintf(query, sizeof(query),
    "INSERT INTO forum_post (content) VALUES ('%s')", user_content);

// ✅ REQUIRED for production
// Option 1: Escape special characters
char escaped_content[8192];
mysql_real_escape_string(conn, escaped_content, user_content, strlen(user_content));
snprintf(query, sizeof(query),
    "INSERT INTO forum_post (content) VALUES ('%s')", escaped_content);

// Option 2: Use prepared statements (recommended)
// Use MySQL C API prepared statements for user content
```

### Password Storage
```c
// ✅ Never store plain text passwords
// In production, use:
// - bcrypt (recommended)
// - argon2
// - scrypt
// - PBKDF2
// Password hash should be 255+ chars to accommodate future algorithms
```

### Input Validation
```c
// ✅ Validate all user inputs
// - Email format
// - Username length and characters
// - Content length limits
// - File upload validation (type, size)
```

---

## Best Practices for Web Tables

### 1. Counter Management
```c
// ✅ Good - Atomic increment for view counts
UPDATE forum_post SET view_count = view_count + 1 WHERE post_id = ?;

// ✅ Good - Increment with validation
UPDATE forum_post SET reply_count = reply_count + 1
WHERE post_id = ? AND is_locked = 'n';
```

### 2. Pagination
```c
// ✅ Good - Always use LIMIT for large result sets
int ForumPost_GetPage(DBConnectionManager* manager, int board_id, int page, int page_size,
                       ForumPost* posts, int* actual_count) {
    int offset = (page - 1) * page_size;
    snprintf(query, sizeof(query),
        "SELECT * FROM forum_post WHERE board_id = %d "
        "ORDER BY created_at DESC LIMIT %d OFFSET %d",
        board_id, page_size, offset);
    // ...
}
```

### 3. Soft Delete
```c
// ✅ Good - Use soft delete for user content
UPDATE forum_post SET is_deleted = 'y', deleted_at = NOW() WHERE post_id = ?;

// Keep data for audit/recovery purposes
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\web_community`
- `taiwan_main_web/` - Official website (364 tables)
- `tw/` - d_taiwan backup (112 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
**Test Databases**: `test_taiwan_main_web`, `test_tw`
**User**: root / 88888888

**⚠️ SECURITY WARNING**: Web community tables handle user-generated content.
**MUST use proper input validation and SQL injection prevention in production.**

**Last Updated**: 2025-11-19
**Version**: v1.0 - Web Community Specific Guide
