#include "member_security_card_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberSecurityCard_Add(DBConnectionManager* manager, const MemberSecurityCard* card) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !card) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_security_card "
        "(m_id, occ_time, phone, cert_key, server_key, card, fail_cnt, re_issue_cnt, "
        "last_issue_time, validity_time, apply_flag, cancel_cnt, web_flag, cert_flag) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', %d, %d, %u, %d, '%c')",
        card->m_id, card->occ_time, card->phone, card->cert_key, card->server_key,
        card->card, card->fail_cnt, card->re_issue_cnt, card->last_issue_time,
        card->validity_time, card->apply_flag, card->cancel_cnt, card->web_flag, card->cert_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_Get(DBConnectionManager* manager, int m_id, MemberSecurityCard* card) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];

    if (!manager || !card) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, phone, cert_key, server_key, card, fail_cnt, re_issue_cnt, "
        "last_issue_time, validity_time, apply_flag, cancel_cnt, web_flag, cert_flag "
        "FROM member_security_card WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(card, 0, sizeof(MemberSecurityCard));
    card->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(card->occ_time, row[1], sizeof(card->occ_time) - 1);
    if (row[2]) strncpy(card->phone, row[2], sizeof(card->phone) - 1);
    if (row[3]) strncpy(card->cert_key, row[3], sizeof(card->cert_key) - 1);
    if (row[4]) strncpy(card->server_key, row[4], sizeof(card->server_key) - 1);
    if (row[5]) strncpy(card->card, row[5], sizeof(card->card) - 1);
    card->fail_cnt = row[6] ? (signed char)atoi(row[6]) : 0;
    card->re_issue_cnt = row[7] ? (signed char)atoi(row[7]) : 0;
    if (row[8]) strncpy(card->last_issue_time, row[8], sizeof(card->last_issue_time) - 1);
    card->validity_time = row[9] ? atoi(row[9]) : 0;
    card->apply_flag = row[10] ? (signed char)atoi(row[10]) : 0;
    card->cancel_cnt = row[11] ? (unsigned short)atoi(row[11]) : 0;
    card->web_flag = row[12] ? (signed char)atoi(row[12]) : 0;
    card->cert_flag = row[13] ? row[13][0] : '0';

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_Update(DBConnectionManager* manager, const MemberSecurityCard* card) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !card) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_security_card SET "
        "occ_time = '%s', phone = '%s', cert_key = '%s', server_key = '%s', card = '%s', "
        "fail_cnt = %d, re_issue_cnt = %d, last_issue_time = '%s', validity_time = %d, "
        "apply_flag = %d, cancel_cnt = %u, web_flag = %d, cert_flag = '%c' "
        "WHERE m_id = %d",
        card->occ_time, card->phone, card->cert_key, card->server_key, card->card,
        card->fail_cnt, card->re_issue_cnt, card->last_issue_time, card->validity_time,
        card->apply_flag, card->cancel_cnt, card->web_flag, card->cert_flag, card->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_security_card WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM member_security_card WHERE m_id = %d LIMIT 1", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int MemberSecurityCard_GetByPhone(DBConnectionManager* manager, const char* phone,
                                   MemberSecurityCard* cards, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[14];
    int count = 0;

    if (!manager || !phone || !cards || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, phone, cert_key, server_key, card, fail_cnt, re_issue_cnt, "
        "last_issue_time, validity_time, apply_flag, cancel_cnt, web_flag, cert_flag "
        "FROM member_security_card WHERE phone = '%s' ORDER BY m_id", phone);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&cards[count], 0, sizeof(MemberSecurityCard));
        cards[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(cards[count].occ_time, row[1], sizeof(cards[count].occ_time) - 1);
        if (row[2]) strncpy(cards[count].phone, row[2], sizeof(cards[count].phone) - 1);
        if (row[3]) strncpy(cards[count].cert_key, row[3], sizeof(cards[count].cert_key) - 1);
        if (row[4]) strncpy(cards[count].server_key, row[4], sizeof(cards[count].server_key) - 1);
        if (row[5]) strncpy(cards[count].card, row[5], sizeof(cards[count].card) - 1);
        cards[count].fail_cnt = row[6] ? (signed char)atoi(row[6]) : 0;
        cards[count].re_issue_cnt = row[7] ? (signed char)atoi(row[7]) : 0;
        if (row[8]) strncpy(cards[count].last_issue_time, row[8], sizeof(cards[count].last_issue_time) - 1);
        cards[count].validity_time = row[9] ? atoi(row[9]) : 0;
        cards[count].apply_flag = row[10] ? (signed char)atoi(row[10]) : 0;
        cards[count].cancel_cnt = row[11] ? (unsigned short)atoi(row[11]) : 0;
        cards[count].web_flag = row[12] ? (signed char)atoi(row[12]) : 0;
        cards[count].cert_flag = row[13] ? row[13][0] : '0';
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_UpdateFailCount(DBConnectionManager* manager, int m_id, signed char fail_cnt) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_security_card SET fail_cnt = %d WHERE m_id = %d", fail_cnt, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_IncrementReissueCount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_security_card SET re_issue_cnt = re_issue_cnt + 1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCard_ResetCard(DBConnectionManager* manager, int m_id, const char* new_card,
                                  const char* new_server_key) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_card || !new_server_key) return -1;

    snprintf(query, sizeof(query),
        "UPDATE member_security_card SET card = '%s', server_key = '%s', "
        "fail_cnt = 0, last_issue_time = NOW() WHERE m_id = %d",
        new_card, new_server_key, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void MemberSecurityCard_PrintInfo(const MemberSecurityCard* card) {
    if (!card) return;

    printf("MemberSecurityCard {\n");
    printf("  m_id: %d\n", card->m_id);
    printf("  occ_time: %s\n", card->occ_time);
    printf("  phone: %s\n", card->phone);
    printf("  cert_key: %s\n", card->cert_key);
    printf("  server_key: %s\n", card->server_key);
    printf("  card: %s\n", card->card);
    printf("  fail_cnt: %d\n", card->fail_cnt);
    printf("  re_issue_cnt: %d\n", card->re_issue_cnt);
    printf("  last_issue_time: %s\n", card->last_issue_time);
    printf("  validity_time: %d\n", card->validity_time);
    printf("  apply_flag: %d\n", card->apply_flag);
    printf("  cancel_cnt: %u\n", card->cancel_cnt);
    printf("  web_flag: %d\n", card->web_flag);
    printf("  cert_flag: %c\n", card->cert_flag);
    printf("}\n");
}
