#include "guild_bbs_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new guild BBS post (gno is auto-generated) */
int GuildBbs_Add(DBConnectionManager* manager, const GuildBbs* bbs) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !bbs) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO guild_bbs (bd_id, empyn, mgno, `open`, main, reg_date, "
        "mod_date, hits, body_type, m_id, reg_id, subject) "
        "VALUES (%u, %u, %d, %u, %u, %u, %u, %u, '%c', %d, '%s', '%s')",
        bbs->bd_id, bbs->empyn, bbs->mgno, bbs->open, bbs->main,
        bbs->reg_date, bbs->mod_date, bbs->hits, bbs->body_type,
        bbs->m_id, bbs->reg_id, bbs->subject);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get guild BBS post by gno */
int GuildBbs_Get(DBConnectionManager* manager, unsigned int gno, GuildBbs* bbs) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !bbs) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, `open`, main, reg_date, mod_date, "
        "hits, body_type, m_id, reg_id, subject FROM guild_bbs WHERE gno = %u",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(bbs, 0, sizeof(GuildBbs));
    bbs->gno = row[0] ? (unsigned int)atoi(row[0]) : 0;
    bbs->bd_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    bbs->empyn = row[2] ? (unsigned char)atoi(row[2]) : 0;
    bbs->mgno = row[3] ? atoi(row[3]) : 0;
    bbs->open = row[4] ? (unsigned char)atoi(row[4]) : 1;
    bbs->main = row[5] ? (unsigned char)atoi(row[5]) : 0;
    bbs->reg_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
    bbs->mod_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
    bbs->hits = row[8] ? (unsigned int)atoi(row[8]) : 0;
    bbs->body_type = row[9] ? row[9][0] : '\0';
    bbs->m_id = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(bbs->reg_id, row[11], sizeof(bbs->reg_id) - 1);
    if (row[12]) strncpy(bbs->subject, row[12], sizeof(bbs->subject) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

/* Update existing guild BBS post */
int GuildBbs_Update(DBConnectionManager* manager, const GuildBbs* bbs) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !bbs) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_bbs SET bd_id = %u, empyn = %u, mgno = %d, `open` = %u, "
        "main = %u, reg_date = %u, mod_date = %u, hits = %u, body_type = '%c', "
        "m_id = %d, reg_id = '%s', subject = '%s' WHERE gno = %u",
        bbs->bd_id, bbs->empyn, bbs->mgno, bbs->open, bbs->main,
        bbs->reg_date, bbs->mod_date, bbs->hits, bbs->body_type,
        bbs->m_id, bbs->reg_id, bbs->subject, bbs->gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete guild BBS post by gno */
int GuildBbs_Delete(DBConnectionManager* manager, unsigned int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM guild_bbs WHERE gno = %u", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if guild BBS post exists */
int GuildBbs_Exists(DBConnectionManager* manager, unsigned int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM guild_bbs WHERE gno = %u", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Get all posts in a board */
int GuildBbs_GetByBoard(DBConnectionManager* manager, unsigned char bd_id,
                        GuildBbs* bbs_list, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, `open`, main, reg_date, mod_date, "
        "hits, body_type, m_id, reg_id, subject FROM guild_bbs "
        "WHERE bd_id = %u ORDER BY gno DESC", bd_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(GuildBbs));
        bbs_list[count].gno = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bbs_list[count].bd_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].empyn = row[2] ? (unsigned char)atoi(row[2]) : 0;
        bbs_list[count].mgno = row[3] ? atoi(row[3]) : 0;
        bbs_list[count].open = row[4] ? (unsigned char)atoi(row[4]) : 1;
        bbs_list[count].main = row[5] ? (unsigned char)atoi(row[5]) : 0;
        bbs_list[count].reg_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        bbs_list[count].mod_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        bbs_list[count].hits = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bbs_list[count].body_type = row[9] ? row[9][0] : '\0';
        bbs_list[count].m_id = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(bbs_list[count].reg_id, row[11], sizeof(bbs_list[count].reg_id) - 1);
        if (row[12]) strncpy(bbs_list[count].subject, row[12], sizeof(bbs_list[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get posts by board and empyn flag */
int GuildBbs_GetByBoardAndEmpyn(DBConnectionManager* manager, unsigned char bd_id,
                                 unsigned char empyn, GuildBbs* bbs_list,
                                 int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, `open`, main, reg_date, mod_date, "
        "hits, body_type, m_id, reg_id, subject FROM guild_bbs "
        "WHERE bd_id = %u AND empyn = %u ORDER BY gno DESC",
        bd_id, empyn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(GuildBbs));
        bbs_list[count].gno = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bbs_list[count].bd_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].empyn = row[2] ? (unsigned char)atoi(row[2]) : 0;
        bbs_list[count].mgno = row[3] ? atoi(row[3]) : 0;
        bbs_list[count].open = row[4] ? (unsigned char)atoi(row[4]) : 1;
        bbs_list[count].main = row[5] ? (unsigned char)atoi(row[5]) : 0;
        bbs_list[count].reg_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        bbs_list[count].mod_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        bbs_list[count].hits = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bbs_list[count].body_type = row[9] ? row[9][0] : '\0';
        bbs_list[count].m_id = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(bbs_list[count].reg_id, row[11], sizeof(bbs_list[count].reg_id) - 1);
        if (row[12]) strncpy(bbs_list[count].subject, row[12], sizeof(bbs_list[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all posts by a member */
int GuildBbs_GetByMember(DBConnectionManager* manager, int m_id,
                         GuildBbs* bbs_list, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !bbs_list || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, bd_id, empyn, mgno, `open`, main, reg_date, mod_date, "
        "hits, body_type, m_id, reg_id, subject FROM guild_bbs "
        "WHERE m_id = %d ORDER BY gno DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&bbs_list[count], 0, sizeof(GuildBbs));
        bbs_list[count].gno = row[0] ? (unsigned int)atoi(row[0]) : 0;
        bbs_list[count].bd_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        bbs_list[count].empyn = row[2] ? (unsigned char)atoi(row[2]) : 0;
        bbs_list[count].mgno = row[3] ? atoi(row[3]) : 0;
        bbs_list[count].open = row[4] ? (unsigned char)atoi(row[4]) : 1;
        bbs_list[count].main = row[5] ? (unsigned char)atoi(row[5]) : 0;
        bbs_list[count].reg_date = row[6] ? (unsigned int)atoi(row[6]) : 0;
        bbs_list[count].mod_date = row[7] ? (unsigned int)atoi(row[7]) : 0;
        bbs_list[count].hits = row[8] ? (unsigned int)atoi(row[8]) : 0;
        bbs_list[count].body_type = row[9] ? row[9][0] : '\0';
        bbs_list[count].m_id = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(bbs_list[count].reg_id, row[11], sizeof(bbs_list[count].reg_id) - 1);
        if (row[12]) strncpy(bbs_list[count].subject, row[12], sizeof(bbs_list[count].subject) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Increment hit counter for a post */
int GuildBbs_IncrementHits(DBConnectionManager* manager, unsigned int gno) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_bbs SET hits = hits + 1 WHERE gno = %u", gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update post subject */
int GuildBbs_UpdateSubject(DBConnectionManager* manager, unsigned int gno,
                            const char* new_subject) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !new_subject) return -1;

    snprintf(query, sizeof(query),
        "UPDATE guild_bbs SET subject = '%s', mod_date = UNIX_TIMESTAMP() "
        "WHERE gno = %u", new_subject, gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print guild BBS post information */
void GuildBbs_PrintInfo(const GuildBbs* bbs) {
    if (!bbs) return;

    printf("Guild BBS Post:\n");
    printf("  gno: %u\n", bbs->gno);
    printf("  bd_id: %u\n", bbs->bd_id);
    printf("  empyn: %u\n", bbs->empyn);
    printf("  mgno: %d\n", bbs->mgno);
    printf("  open: %u\n", bbs->open);
    printf("  main: %u\n", bbs->main);
    printf("  reg_date: %u\n", bbs->reg_date);
    printf("  mod_date: %u\n", bbs->mod_date);
    printf("  hits: %u\n", bbs->hits);
    printf("  body_type: %c\n", bbs->body_type);
    printf("  m_id: %d\n", bbs->m_id);
    printf("  reg_id: %s\n", bbs->reg_id);
    printf("  subject: %s\n", bbs->subject);
}