# Commerce & Trade ORM Development Quick Guide

**3 Database Modules | 69 Tables | Current Progress: 0%**

---

## Quick Index
- First time development? → [Standard Development Process](#standard-development-process)
- Need code templates? → [Development Templates](#development-templates)
- Encountered errors? → [Common Errors](#common-errors)
- Need command reference? → [Quick Development Script](#quick-development-script)

---

## Commerce & Trade Module Overview

### 3 Database Modules (69 Tables)

| Database Module | Tables | Focus Area | DB Type Constant |
|----------------|--------|------------|------------------|
| `taiwan_prod/` | 35 | Product management & cash shop | `DB_TYPE_CAIN = 2` |
| `taiwan_cain_auction_cera/` | 17 | Cera auction house | `DB_TYPE_CAIN = 2` |
| `taiwan_cain_auction_gold/` | 17 | Gold auction house | `DB_TYPE_CAIN = 2` |

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

---

## Commerce Table Characteristics

Commerce tables typically have:
- **Product information** (product_id, product_name, price)
- **Transaction records** (order_id, buyer_id, seller_id, amount)
- **Auction data** (auction_id, bid_amount, bid_time)
- **Inventory management** (quantity, stock_count)
- **Currency types** (cera points, gold coins)
- **Status tracking** (order_status, payment_status)

---

## Development Templates

### Template 1: Product Table

**Typical Structure**:
```sql
CREATE TABLE product_info (
  product_id int(11) NOT NULL auto_increment,
  product_code varchar(50) NOT NULL,
  product_name varchar(200),
  price int(11) NOT NULL default 0,
  stock_quantity int(11) default 0,
  is_available enum('y','n') default 'y',
  PRIMARY KEY (product_id),
  UNIQUE KEY (product_code)
);
```

**Header File** `product_info_orm.h`:
```c
#ifndef PRODUCT_INFO_ORM_H
#define PRODUCT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int product_id;
    char product_code[51];
    char product_name[201];
    int price;
    int stock_quantity;
    char is_available;  // 'y' or 'n'
} ProductInfo;

/* CRUD Operations */
int ProductInfo_Add(DBConnectionManager* manager, const ProductInfo* product);
int ProductInfo_Get(DBConnectionManager* manager, unsigned int product_id, ProductInfo* product);
int ProductInfo_GetByCode(DBConnectionManager* manager, const char* product_code, ProductInfo* product);
int ProductInfo_Update(DBConnectionManager* manager, const ProductInfo* product);
int ProductInfo_Delete(DBConnectionManager* manager, unsigned int product_id);

/* Business Operations */
int ProductInfo_GetAvailableProducts(DBConnectionManager* manager, ProductInfo* products,
                                      int max_count, int* actual_count);
int ProductInfo_UpdateStock(DBConnectionManager* manager, unsigned int product_id, int quantity_change);
int ProductInfo_SetAvailable(DBConnectionManager* manager, unsigned int product_id, char is_available);

void ProductInfo_PrintInfo(const ProductInfo* product);

#endif
```

---

### Template 2: Auction Table

**Typical Structure**:
```sql
CREATE TABLE auction_listing (
  auction_id bigint(20) NOT NULL auto_increment,
  seller_charac_no int(11) NOT NULL,
  item_id int(11) NOT NULL,
  item_name varchar(200),
  start_price int(11) NOT NULL,
  buyout_price int(11),
  current_bid int(11) default 0,
  bidder_charac_no int(11) default 0,
  list_time datetime,
  expire_time datetime,
  status enum('active','sold','expired') default 'active',
  PRIMARY KEY (auction_id),
  KEY idx_seller (seller_charac_no),
  KEY idx_status_expire (status, expire_time)
);
```

**Key Implementation**:
```c
typedef struct {
    unsigned long long auction_id;
    int seller_charac_no;
    int item_id;
    char item_name[201];
    int start_price;
    int buyout_price;
    int current_bid;
    int bidder_charac_no;
    char list_time[20];
    char expire_time[20];
    char status;  // 'a' = active, 's' = sold, 'e' = expired
} AuctionListing;

int AuctionListing_PlaceBid(DBConnectionManager* manager,
                             unsigned long long auction_id,
                             int bidder_charac_no,
                             int bid_amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    // Update bid if amount is higher than current bid
    snprintf(query, sizeof(query),
        "UPDATE auction_listing "
        "SET current_bid = %d, bidder_charac_no = %d "
        "WHERE auction_id = %llu AND status = 'active' AND current_bid < %d "
        "AND expire_time > NOW()",
        bid_amount, bidder_charac_no, auction_id, bid_amount);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionListing_GetActiveAuctions(DBConnectionManager* manager,
                                      AuctionListing* auctions,
                                      int max_count,
                                      int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !auctions || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT auction_id, seller_charac_no, item_id, item_name, "
        "start_price, buyout_price, current_bid, bidder_charac_no, "
        "list_time, expire_time, status "
        "FROM auction_listing "
        "WHERE status = 'active' AND expire_time > NOW() "
        "ORDER BY list_time DESC LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&auctions[count], 0, sizeof(AuctionListing));
        auctions[count].auction_id = row[0] ? (unsigned long long)atoll(row[0]) : 0;
        auctions[count].seller_charac_no = row[1] ? atoi(row[1]) : 0;
        auctions[count].item_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(auctions[count].item_name, row[3], sizeof(auctions[count].item_name) - 1);
        auctions[count].start_price = row[4] ? atoi(row[4]) : 0;
        auctions[count].buyout_price = row[5] ? atoi(row[5]) : 0;
        auctions[count].current_bid = row[6] ? atoi(row[6]) : 0;
        auctions[count].bidder_charac_no = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(auctions[count].list_time, row[8], sizeof(auctions[count].list_time) - 1);
        if (row[9]) strncpy(auctions[count].expire_time, row[9], sizeof(auctions[count].expire_time) - 1);
        auctions[count].status = row[10] ? row[10][0] : 'a';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
```

---

## Common Errors

### Error 1: Currency Amount Type
```c
// ❌ Wrong - using unsigned char for large amounts
unsigned char price;  // Max 255, too small for currency

// ✅ Correct - use int or long for currency
int price;  // Can handle reasonable amounts
unsigned long long total_sales;  // For cumulative amounts
```

### Error 2: BIGINT for Auction IDs
```c
// ❌ Wrong - using int for bigint(20)
int auction_id = atoi(row[0]);

// ✅ Correct - use unsigned long long and atoll
unsigned long long auction_id = atoll(row[0]);
```

### Error 3: Race Condition in Bidding
```c
// ❌ Wrong - no validation of current bid
UPDATE auction_listing SET current_bid = 1000 WHERE auction_id = 123;

// ✅ Correct - validate bid is higher than current
UPDATE auction_listing SET current_bid = 1000
WHERE auction_id = 123 AND current_bid < 1000;
```

### Error 4: Transaction Status Enum
```c
// In C struct, use char for ENUM
char status;  // 'a' = active, 's' = sold, 'e' = expired

// In SQL query, use full string
snprintf(query, sizeof(query),
    "INSERT INTO auction (..., status) VALUES (..., 'active')");

// When parsing from result
auction->status = row[10] ? row[10][0] : 'a';  // Take first character
```

---

## Standard Development Process

### Step 0: Find Table Structure

```bash
# Search taiwan_prod
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_prod -e "SHOW TABLES" | grep -i <table_name>'

# Search taiwan_cain_auction_cera
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_auction_cera -e "SHOW TABLES" | grep -i <table_name>'

# Search taiwan_cain_auction_gold
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 taiwan_cain_auction_gold -e "SHOW TABLES" | grep -i <table_name>'

# Get structure
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 <database> -e "SHOW CREATE TABLE <table>\\G"'
```

### Step 1: Create Test Table

```bash
# Create test table (use appropriate test database)
ssh vm131 'mysql -h192.168.200.131 -uroot -p88888888 test_<database> -e "
  CREATE TABLE IF NOT EXISTS <table> (
    -- paste structure from Step 0
  ) ENGINE=InnoDB DEFAULT CHARSET=utf8"'
```

---

## Quick Development Script

```bash
#!/bin/bash
DATABASE=$1  # taiwan_prod, taiwan_cain_auction_cera, taiwan_cain_auction_gold
TABLE=$2

if [ -z "$DATABASE" ] || [ -z "$TABLE" ]; then
    echo "Usage: ./dev_commerce_module.sh <database> <table_name>"
    echo "Example: ./dev_commerce_module.sh taiwan_prod product_info"
    echo ""
    echo "Available databases:"
    echo "  - taiwan_prod (35 tables)"
    echo "  - taiwan_cain_auction_cera (17 tables)"
    echo "  - taiwan_cain_auction_gold (17 tables)"
    exit 1
fi

echo "=== Developing Commerce Module: $DATABASE/$TABLE ==="
cd "C:/Users/waw/Desktop/local_orm"

# Upload
echo "[1/5] Uploading..."
scp "commerce_trade/${DATABASE}/headers/${TABLE}_orm.h" vm131:/tmp/dnf_new_orm/headers/ &
scp "commerce_trade/${DATABASE}/sources/${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/sources/ &
scp "commerce_trade/${DATABASE}/tests/test_${TABLE}_orm.c" vm131:/tmp/dnf_new_orm/tests/ &
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

echo "[5/5] Complete! Update commerce_trade/ALL_DATABASES_TABLES.md"
```

---

## Quick Reference

### Database Type
```c
DB_TYPE_CAIN = 2  // All three commerce databases
```

### Test Databases
```
test_taiwan_prod
test_taiwan_cain_auction_cera
test_taiwan_cain_auction_gold
```

### Development Checklist
```
□ Used DB_TYPE_CAIN for all operations
□ BIGINT auction IDs use unsigned long long and atoll()
□ Currency amounts use int or larger types
□ Bid validation includes current_bid check
□ ENUM status fields use char type
□ Expire time comparisons use NOW()
□ Test files use English-only comments
□ All results are freed
□ Test data cleaned up
```

---

## Project Information

**Local Path**: `C:\Users\waw\Desktop\local_orm\commerce_trade`
- `taiwan_prod/` - Product management (35 tables)
- `taiwan_cain_auction_cera/` - Cera auction (17 tables)
- `taiwan_cain_auction_gold/` - Gold auction (17 tables)
- `ALL_DATABASES_TABLES.md` - Progress tracking

**Server**: vm131 (192.168.200.131)
**Database**: MySQL @ 192.168.200.131:3306
**User**: root / 88888888

**Last Updated**: 2025-11-19
**Version**: v1.0 - Commerce & Trade Specific Guide
