#include "member_security_card_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_basic_crud(DBConnectionManager* manager);
void test_business_operations(DBConnectionManager* manager);
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

    if (DBConnectionManager_Connect(&manager, DB_TYPE_LOGIN) < 0) {
        printf("Failed to connect to database\n");
        DBConnectionManager_Cleanup(&manager);
        return 1;
    }

    printf("=== MemberSecurityCard ORM Test Suite ===\n\n");

    cleanup_test_data(&manager);
    test_basic_crud(&manager);
    test_business_operations(&manager);
    cleanup_test_data(&manager);

    DBConnectionManager_Cleanup(&manager);
    printf("\n=== All Tests Completed ===\n");
    return 0;
}

void cleanup_test_data(DBConnectionManager* manager) {
    DBQueryResult result;
    char query[256];

    snprintf(query, sizeof(query),
        "DELETE FROM member_security_card WHERE m_id IN (300001, 300002, 300003)");

    DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result);
    DBQueryResult_Free(&result);
}

void test_basic_crud(DBConnectionManager* manager) {
    MemberSecurityCard card;
    MemberSecurityCard retrieved;

    printf("[TEST] Basic CRUD Operations\n");

    /* Add test */
    memset(&card, 0, sizeof(MemberSecurityCard));
    card.m_id = 300001;
    strcpy(card.occ_time, "2025-01-01 10:00:00");
    strcpy(card.phone, "12345678901");
    strcpy(card.cert_key, "CERT12345678");
    strcpy(card.server_key, "SERVERKEY123456789012345678901");
    strcpy(card.card, "CARDDATA1234567890");
    card.fail_cnt = 0;
    card.re_issue_cnt = 0;
    strcpy(card.last_issue_time, "2025-01-01 10:00:00");
    card.validity_time = 3600;
    card.apply_flag = 1;
    card.cancel_cnt = 0;
    card.web_flag = 0;
    card.cert_flag = '0';

    if (MemberSecurityCard_Add(manager, &card) == 0) {
        printf("  [PASS] Add security card\n");
    } else {
        printf("  [FAIL] Add security card\n");
    }

    /* Exists test */
    if (MemberSecurityCard_Exists(manager, 300001)) {
        printf("  [PASS] Security card exists\n");
    } else {
        printf("  [FAIL] Security card should exist\n");
    }

    /* Get test */
    memset(&retrieved, 0, sizeof(MemberSecurityCard));
    if (MemberSecurityCard_Get(manager, 300001, &retrieved) == 0) {
        printf("  [PASS] Get security card\n");
        if (retrieved.m_id == 300001 && strcmp(retrieved.phone, "12345678901") == 0) {
            printf("  [PASS] Data verification\n");
        } else {
            printf("  [FAIL] Data mismatch\n");
        }
    } else {
        printf("  [FAIL] Get security card\n");
    }

    /* Update test */
    strcpy(retrieved.phone, "98765432109");
    retrieved.fail_cnt = 1;
    if (MemberSecurityCard_Update(manager, &retrieved) == 0) {
        printf("  [PASS] Update security card\n");
    } else {
        printf("  [FAIL] Update security card\n");
    }

    /* Delete test */
    if (MemberSecurityCard_Delete(manager, 300001) == 0) {
        printf("  [PASS] Delete security card\n");
    } else {
        printf("  [FAIL] Delete security card\n");
    }

    if (!MemberSecurityCard_Exists(manager, 300001)) {
        printf("  [PASS] Verify deletion\n");
    } else {
        printf("  [FAIL] Record should not exist after deletion\n");
    }

    printf("\n");
}

void test_business_operations(DBConnectionManager* manager) {
    MemberSecurityCard cards[10];
    int count;

    printf("[TEST] Business Operations\n");

    /* Add multiple cards with same phone */
    MemberSecurityCard c1 = {300002, "2025-01-01 10:00:00", "11111111111", "CERT1", "KEY1",
                              "CARD1", 0, 0, "2025-01-01 10:00:00", 3600, 1, 0, 0, '0'};
    MemberSecurityCard c2 = {300003, "2025-01-01 11:00:00", "11111111111", "CERT2", "KEY2",
                              "CARD2", 0, 0, "2025-01-01 11:00:00", 3600, 1, 0, 0, '0'};

    MemberSecurityCard_Add(manager, &c1);
    MemberSecurityCard_Add(manager, &c2);

    /* Test GetByPhone */
    memset(cards, 0, sizeof(cards));
    if (MemberSecurityCard_GetByPhone(manager, "11111111111", cards, 10, &count) == 0) {
        printf("  [PASS] GetByPhone (found %d records)\n", count);
        if (count == 2) {
            printf("  [PASS] Correct record count\n");
        } else {
            printf("  [FAIL] Expected 2 records, got %d\n", count);
        }
    } else {
        printf("  [FAIL] GetByPhone\n");
    }

    /* Test UpdateFailCount */
    if (MemberSecurityCard_UpdateFailCount(manager, 300002, 3) == 0) {
        printf("  [PASS] UpdateFailCount\n");

        MemberSecurityCard card;
        if (MemberSecurityCard_Get(manager, 300002, &card) == 0 && card.fail_cnt == 3) {
            printf("  [PASS] Verify fail count updated\n");
        } else {
            printf("  [FAIL] Fail count not updated correctly\n");
        }
    } else {
        printf("  [FAIL] UpdateFailCount\n");
    }

    /* Test IncrementReissueCount */
    if (MemberSecurityCard_IncrementReissueCount(manager, 300003) == 0) {
        printf("  [PASS] IncrementReissueCount\n");

        MemberSecurityCard card;
        if (MemberSecurityCard_Get(manager, 300003, &card) == 0 && card.re_issue_cnt == 1) {
            printf("  [PASS] Verify reissue count incremented\n");
        } else {
            printf("  [FAIL] Reissue count not incremented\n");
        }
    } else {
        printf("  [FAIL] IncrementReissueCount\n");
    }

    /* Test ResetCard */
    if (MemberSecurityCard_ResetCard(manager, 300002, "NEWCARD123", "NEWKEY123") == 0) {
        printf("  [PASS] ResetCard\n");

        MemberSecurityCard card;
        if (MemberSecurityCard_Get(manager, 300002, &card) == 0 &&
            strcmp(card.card, "NEWCARD123") == 0 && card.fail_cnt == 0) {
            printf("  [PASS] Verify card reset\n");
        } else {
            printf("  [FAIL] Card not reset correctly\n");
        }
    } else {
        printf("  [FAIL] ResetCard\n");
    }

    printf("\n");
}
