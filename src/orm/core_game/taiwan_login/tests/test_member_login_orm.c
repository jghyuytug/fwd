/**
 * DNF Game Server - MemberLogin ORM Test Program
 *
 * Features:
 * 1. Test login session CRUD operations
 * 2. Test security features (fail count, IP tracking)
 * 3. Test gold limits and anti-cheat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db_connection_manager.h"
#include "member_login_orm.h"

/* ANSI color codes */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

void print_separator() {
    printf("%s================================================================================\n%s",
           COLOR_CYAN, COLOR_RESET);
}

void print_success(const char* msg) {
    printf("%s✓ %s%s\n", COLOR_GREEN, msg, COLOR_RESET);
}

void print_error(const char* msg) {
    printf("%s✗ %s%s\n", COLOR_RED, msg, COLOR_RESET);
}

void print_info(const char* msg) {
    printf("%s● %s%s\n", COLOR_BLUE, msg, COLOR_RESET);
}

/* Test 1: Create login sessions */
int test_create_login(DBConnectionManager* manager) {
    MemberLogin login;
    unsigned int current_time = (unsigned int)time(NULL);

    print_separator();
    printf("%sTest 1: Create Login Sessions%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Create login session 1 */
    print_info("Creating login session for m_id=1");
    MemberLogin_Init(&login);
    login.m_id = 1;
    login.login_time = current_time;
    login.expire_time = current_time + 3600; /* Expires in 1 hour */
    login.last_play_time = current_time;
    strcpy(login.login_ip, "192.168.1.100");
    login.security_flag = 1;
    login.trade_gold_daily = 1000000;

    if (MemberLogin_Create(manager, &login) == 0) {
        print_success("Login session 1 created successfully");
        printf("  Member ID: %d\n", login.m_id);
        printf("  Login IP: %s\n", login.login_ip);
        printf("  Expire Time: %u\n", login.expire_time);
    } else {
        print_error("Failed to create login session 1");
        return -1;
    }

    /* Create login session 2 */
    print_info("Creating login session for m_id=2");
    MemberLogin_Init(&login);
    login.m_id = 2;
    login.login_time = current_time - 600; /* 10 minutes ago */
    login.expire_time = current_time + 7200; /* Expires in 2 hours */
    login.last_play_time = current_time - 300;
    strcpy(login.login_ip, "192.168.1.101");
    login.account_fail = 2; /* Some failures */
    login.total_account_fail = 5;

    if (MemberLogin_Create(manager, &login) == 0) {
        print_success("Login session 2 created successfully");
        printf("  Member ID: %d\n", login.m_id);
        printf("  Fail Count: %d / %u\n", login.account_fail, login.total_account_fail);
    } else {
        print_error("Failed to create login session 2");
        return -1;
    }

    /* Create login session 3 (same IP as session 1) */
    print_info("Creating login session for m_id=3 (same IP as m_id=1)");
    MemberLogin_Init(&login);
    login.m_id = 3;
    login.login_time = current_time;
    login.expire_time = current_time + 1800;
    login.last_play_time = current_time;
    strcpy(login.login_ip, "192.168.1.100"); /* Same IP */
    login.security_flag = 2; /* Flagged for same IP */

    if (MemberLogin_Create(manager, &login) == 0) {
        print_success("Login session 3 created successfully");
        printf("  Member ID: %d\n", login.m_id);
        printf("  Login IP: %s (shared)\n", login.login_ip);
    } else {
        print_error("Failed to create login session 3");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 2: Query login sessions */
int test_query_login(DBConnectionManager* manager) {
    MemberLogin login;

    print_separator();
    printf("%sTest 2: Query Login Sessions%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Query m_id=1 */
    print_info("Query login session for m_id=1");
    MemberLogin_Init(&login);

    if (MemberLogin_GetByID(manager, 1, &login) == 0) {
        print_success("Query successful");
        printf("  Member ID: %d\n", login.m_id);
        printf("  Login IP: %s\n", login.login_ip);
        printf("  Login Time: %u\n", login.login_time);
        printf("  Expire Time: %u\n", login.expire_time);
        printf("  Trade Gold: %u\n", login.trade_gold_daily);
    } else {
        print_error("Query failed");
        return -1;
    }

    /* Query m_id=2 */
    print_info("Query login session for m_id=2");
    MemberLogin_Init(&login);

    if (MemberLogin_GetByID(manager, 2, &login) == 0) {
        print_success("Query successful");
        printf("  Member ID: %d\n", login.m_id);
        printf("  Current Fail: %d\n", login.account_fail);
        printf("  Total Fail: %u\n", login.total_account_fail);
    } else {
        print_error("Query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 3: Update login session */
int test_update_login(DBConnectionManager* manager) {
    MemberLogin login;

    print_separator();
    printf("%sTest 3: Update Login Session%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get original data */
    print_info("Getting original login data for m_id=1");
    if (MemberLogin_GetByID(manager, 1, &login) != 0) {
        print_error("Failed to get original data");
        return -1;
    }

    unsigned int old_gold = login.trade_gold_daily;
    printf("  Original trade gold: %u\n", old_gold);

    /* Update data */
    print_info("Updating login session");
    login.trade_gold_daily = 2000000;
    login.dungeon_gain_gold = 500000;

    if (MemberLogin_Update(manager, &login) == 0) {
        print_success("Update successful");
        printf("  New trade gold: %u\n", login.trade_gold_daily);
        printf("  Dungeon gold: %u\n", login.dungeon_gain_gold);
    } else {
        print_error("Update failed");
        return -1;
    }

    /* Verify update */
    print_info("Verifying update");
    MemberLogin_Init(&login);

    if (MemberLogin_GetByID(manager, 1, &login) == 0) {
        if (login.trade_gold_daily == 2000000 && login.dungeon_gain_gold == 500000) {
            print_success("Update verification passed");
        } else {
            print_error("Update verification failed");
            return -1;
        }
    } else {
        print_error("Verification query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 4: Existence check */
int test_existence_check(DBConnectionManager* manager) {
    print_separator();
    printf("%sTest 4: Existence Check%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Check existing session */
    print_info("Check if m_id=1 exists");
    if (MemberLogin_Exists(manager, 1)) {
        print_success("Correctly identified session exists");
    } else {
        print_error("Incorrectly identified session does not exist");
        return -1;
    }

    /* Check non-existing session */
    print_info("Check if m_id=999 does not exist");
    if (!MemberLogin_Exists(manager, 999)) {
        print_success("Correctly identified session does not exist");
    } else {
        print_error("Incorrectly identified session exists");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 5: Update login time */
int test_update_login_time(DBConnectionManager* manager) {
    MemberLogin login;
    unsigned int old_time;

    print_separator();
    printf("%sTest 5: Update Login Time%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Get original time */
    if (MemberLogin_GetByID(manager, 1, &login) != 0) {
        print_error("Failed to get original data");
        return -1;
    }
    old_time = login.login_time;

    /* Update login time */
    print_info("Updating login time for m_id=1");
    if (MemberLogin_UpdateLoginTime(manager, 1, "192.168.1.200") == 0) {
        print_success("Login time updated successfully");

        /* Verify */
        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 1, &login) == 0) {
            if (login.login_time >= old_time && strcmp(login.login_ip, "192.168.1.200") == 0) {
                print_success("Update verification passed");
                printf("  New IP: %s\n", login.login_ip);
            } else {
                print_error("Update verification failed");
                return -1;
            }
        }
    } else {
        print_error("Login time update failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 6: Failure count management */
int test_failure_count(DBConnectionManager* manager) {
    MemberLogin login;

    print_separator();
    printf("%sTest 6: Failure Count Management%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Increment failure */
    print_info("Incrementing failure count for m_id=2");
    if (MemberLogin_IncrementFailCount(manager, 2) == 0) {
        print_success("Failure count incremented");

        /* Verify */
        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 2, &login) == 0) {
            printf("  Current fail: %d\n", login.account_fail);
            printf("  Total fail: %u\n", login.total_account_fail);

            if (login.account_fail == 3 && login.total_account_fail == 6) {
                print_success("Increment verification passed");
            } else {
                print_error("Increment verification failed");
                return -1;
            }
        }
    } else {
        print_error("Failure count increment failed");
        return -1;
    }

    /* Reset failure */
    print_info("Resetting failure count for m_id=2");
    if (MemberLogin_ResetFailCount(manager, 2) == 0) {
        print_success("Failure count reset");

        /* Verify */
        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 2, &login) == 0) {
            if (login.account_fail == 0) {
                print_success("Reset verification passed");
            } else {
                print_error("Reset verification failed");
                return -1;
            }
        }
    } else {
        print_error("Failure count reset failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 7: Gold management */
int test_gold_management(DBConnectionManager* manager) {
    MemberLogin login;

    print_separator();
    printf("%sTest 7: Gold Management%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Update trade gold */
    print_info("Updating trade gold for m_id=1");
    if (MemberLogin_UpdateTradeGold(manager, 1, 5000000) == 0) {
        print_success("Trade gold updated");

        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 1, &login) == 0) {
            printf("  Trade gold: %u\n", login.trade_gold_daily);
        }
    } else {
        print_error("Trade gold update failed");
        return -1;
    }

    /* Add dungeon gold */
    print_info("Adding dungeon gold for m_id=1");
    if (MemberLogin_AddDungeonGold(manager, 1, 100000) == 0) {
        print_success("Dungeon gold added");

        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 1, &login) == 0) {
            printf("  Dungeon gold: %u\n", login.dungeon_gain_gold);
            if (login.dungeon_gain_gold == 600000) { /* 500000 + 100000 */
                print_success("Addition verification passed");
            }
        }
    } else {
        print_error("Dungeon gold addition failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 8: Security flag */
int test_security_flag(DBConnectionManager* manager) {
    MemberLogin login;

    print_separator();
    printf("%sTest 8: Security Flag%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Updating security flag for m_id=3");
    if (MemberLogin_UpdateSecurityFlag(manager, 3, 5) == 0) {
        print_success("Security flag updated");

        MemberLogin_Init(&login);
        if (MemberLogin_GetByID(manager, 3, &login) == 0) {
            if (login.security_flag == 5) {
                print_success("Security flag verification passed");
                printf("  Security flag: %d\n", login.security_flag);
            } else {
                print_error("Security flag verification failed");
                return -1;
            }
        }
    } else {
        print_error("Security flag update failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 9: IP tracking */
int test_ip_tracking(DBConnectionManager* manager) {
    int m_ids[10];

    print_separator();
    printf("%sTest 9: IP Tracking (Security)%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Getting all sessions from IP 192.168.1.100");
    int count = MemberLogin_GetByLoginIP(manager, "192.168.1.100", m_ids, 10);

    if (count >= 0) {
        print_success("IP tracking query successful");
        printf("  Sessions from this IP: %d\n", count);

        for (int i = 0; i < count; i++) {
            printf("    Member ID: %d\n", m_ids[i]);
        }

        if (count >= 2) {
            print_success("Detected multiple accounts from same IP");
        }
    } else {
        print_error("IP tracking query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 10: Statistics */
int test_statistics(DBConnectionManager* manager) {
    unsigned int current_time = (unsigned int)time(NULL);

    print_separator();
    printf("%sTest 10: Statistics%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    /* Total count */
    print_info("Getting total session count");
    int total = MemberLogin_GetCount(manager);
    if (total >= 0) {
        print_success("Total count query successful");
        printf("  Total sessions: %d\n", total);
    } else {
        print_error("Total count query failed");
        return -1;
    }

    /* Active count */
    print_info("Getting active session count");
    int active = MemberLogin_GetActiveCount(manager, current_time);
    if (active >= 0) {
        print_success("Active count query successful");
        printf("  Active sessions: %d\n", active);
    } else {
        print_error("Active count query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 11: High failure accounts */
int test_high_failure(DBConnectionManager* manager) {
    MemberLogin logins[10];

    print_separator();
    printf("%sTest 11: High Failure Accounts (Security)%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Getting accounts with failure count >= 3");
    int count = MemberLogin_GetHighFailureAccounts(manager, 3, logins, 10);

    if (count >= 0) {
        print_success("High failure query successful");
        printf("  High failure accounts: %d\n", count);

        for (int i = 0; i < count; i++) {
            printf("    m_id=%d: fail=%d\n", logins[i].m_id, logins[i].account_fail);
        }
    } else {
        print_error("High failure query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 12: Recently logged in */
int test_recently_logged_in(DBConnectionManager* manager) {
    MemberLogin logins[10];

    print_separator();
    printf("%sTest 12: Recently Logged In%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Getting top 5 recently logged in members");
    int count = MemberLogin_GetRecentlyLoggedIn(manager, logins, 5);

    if (count >= 0) {
        print_success("Recently logged in query successful");
        printf("  Found %d sessions\n", count);

        for (int i = 0; i < count; i++) {
            printf("    %d. m_id=%d (login_time: %u)\n", i + 1, logins[i].m_id, logins[i].login_time);
        }
    } else {
        print_error("Recently logged in query failed");
        return -1;
    }

    printf("\n");
    return 0;
}

/* Test 13: Delete session */
int test_delete_session(DBConnectionManager* manager) {
    print_separator();
    printf("%sTest 13: Delete Session%s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();

    print_info("Deleting session for m_id=3");
    if (MemberLogin_Delete(manager, 3) == 0) {
        print_success("Session deleted successfully");

        /* Verify deletion */
        if (!MemberLogin_Exists(manager, 3)) {
            print_success("Deletion verification passed");
        } else {
            print_error("Session still exists after deletion");
            return -1;
        }
    } else {
        print_error("Delete failed");
        return -1;
    }

    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    DBConnectionManager manager;

    printf("\n");
    print_separator();
    printf("%s  DNF Game Server - MemberLogin ORM Test  %s\n", COLOR_CYAN, COLOR_RESET);
    print_separator();
    printf("\n");

    if (argc < 2) {
        print_error("Usage: test_member_login_orm <config_file>");
        printf("  Example: ./test_member_login_orm ../config/database.cfg\n");
        return 1;
    }

    const char* config_file = argv[1];
    printf("Config file: %s\n\n", config_file);

    /* Initialize database connection */
    memset(&manager, 0, sizeof(DBConnectionManager));

    print_info("Initializing database connection...");
    if (DBConnectionManager_Initialize(&manager, config_file) != 0) {
        print_error("Initialization failed");
        return 1;
    }
    print_success("Initialization successful");

    /* Connect to login database */
    print_info("Connecting to login database (test_taiwan_login)...");
    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) != 0) {
        print_error("Connection failed");
        return 1;
    }
    print_success("Connection successful");
    printf("\n");

    /* Run tests */
    int success_count = 0;
    int total_tests = 13;

    if (test_create_login(&manager) == 0) success_count++;
    if (test_query_login(&manager) == 0) success_count++;
    if (test_update_login(&manager) == 0) success_count++;
    if (test_existence_check(&manager) == 0) success_count++;
    if (test_update_login_time(&manager) == 0) success_count++;
    if (test_failure_count(&manager) == 0) success_count++;
    if (test_gold_management(&manager) == 0) success_count++;
    if (test_security_flag(&manager) == 0) success_count++;
    if (test_ip_tracking(&manager) == 0) success_count++;
    if (test_statistics(&manager) == 0) success_count++;
    if (test_high_failure(&manager) == 0) success_count++;
    if (test_recently_logged_in(&manager) == 0) success_count++;
    if (test_delete_session(&manager) == 0) success_count++;

    /* Cleanup */
    print_info("Cleaning up database connection...");
    DBConnectionManager_Cleanup(&manager);
    print_success("Cleanup complete");

    printf("\n");
    print_separator();
    printf("%sTest Result: %d/%d Passed%s\n",
           success_count == total_tests ? COLOR_GREEN : COLOR_YELLOW,
           success_count, total_tests, COLOR_RESET);
    print_separator();
    printf("\n");

    return (success_count == total_tests) ? 0 : 1;
}
