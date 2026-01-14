/**
 * @file charac_quest_orm.c
 * @brief Character Quest ORM Module Implementation
 */

#include "charac_quest_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Helper function to convert binary data to hex string */
static void binary_to_hex(const unsigned char* bin, int bin_len, char* hex) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sprintf(hex + (i * 2), "%02X", bin[i]);
    }
    hex[bin_len * 2] = '\0';
}

/* Helper function to convert hex string to binary data */
static void hex_to_binary(const char* hex, unsigned char* bin, int bin_len) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sscanf(hex + (i * 2), "%2hhX", &bin[i]);
    }
}

int CharacQuest_Create(DBConnectionManager* manager, CharacQuest* quest) {
    char query[8192];
    char hex_quest_10[129], hex_quest_15[129], hex_quest_20[129];
    char hex_quest_30[129], hex_quest_40[129], hex_quest_40_ext[129];
    char hex_quest_50[129], hex_quest_60[129], hex_quest_70[129], hex_quest_etc[129];
    char hex_quest_50_ext[129], hex_quest_60_ext[129];
    char hex_quest_etc_ext[129], hex_quest_60_ext_2nd[129];
    DBQueryResult qresult;
    int ret;

    if (!manager || !quest) return -1;

    /* Convert binary data to hex strings */
    binary_to_hex(quest->quest_10, QUEST_DATA_SIZE, hex_quest_10);
    binary_to_hex(quest->quest_15, QUEST_DATA_SIZE, hex_quest_15);
    binary_to_hex(quest->quest_20, QUEST_DATA_SIZE, hex_quest_20);
    binary_to_hex(quest->quest_30, QUEST_DATA_SIZE, hex_quest_30);
    binary_to_hex(quest->quest_40, QUEST_DATA_SIZE, hex_quest_40);
    binary_to_hex(quest->quest_40_ext, QUEST_DATA_SIZE, hex_quest_40_ext);
    binary_to_hex(quest->quest_50, QUEST_DATA_SIZE, hex_quest_50);
    binary_to_hex(quest->quest_60, QUEST_DATA_SIZE, hex_quest_60);
    binary_to_hex(quest->quest_70, QUEST_DATA_SIZE, hex_quest_70);
    binary_to_hex(quest->quest_etc, QUEST_DATA_SIZE, hex_quest_etc);
    binary_to_hex(quest->quest_50_ext, QUEST_DATA_SIZE, hex_quest_50_ext);
    binary_to_hex(quest->quest_60_ext, QUEST_DATA_SIZE, hex_quest_60_ext);
    binary_to_hex(quest->quest_etc_ext, QUEST_DATA_SIZE, hex_quest_etc_ext);
    binary_to_hex(quest->quest_60_ext_2nd, QUEST_DATA_SIZE, hex_quest_60_ext_2nd);

    snprintf(query, sizeof(query),
             "INSERT INTO charac_quest ("
             "charac_no, quest_10, quest_15, quest_20, quest_30, quest_40, quest_40_ext, "
             "quest_50, quest_60, quest_70, quest_etc, "
             "play_1, play_1_trigger, play_2, play_2_trigger, play_3, play_3_trigger, "
             "play_4, play_4_trigger, play_5, play_5_trigger, play_6, play_6_trigger, "
             "play_7, play_7_trigger, play_8, play_8_trigger, play_9, play_9_trigger, "
             "play_10, play_10_trigger, "
             "quest_50_ext, quest_60_ext, quest_etc_ext, quest_60_ext_2nd"
             ") VALUES ("
             "%d, UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), "
             "UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), "
             "%u, %d, %u, %d, %u, %d, %u, %d, %u, %d, "
             "%u, %d, %u, %d, %u, %d, %u, %d, %u, %d, "
             "UNHEX('%s'), UNHEX('%s'), UNHEX('%s'), UNHEX('%s')"
             ")",
             quest->charac_no,
             hex_quest_10, hex_quest_15, hex_quest_20, hex_quest_30, hex_quest_40, hex_quest_40_ext,
             hex_quest_50, hex_quest_60, hex_quest_70, hex_quest_etc,
             quest->play_1, quest->play_1_trigger, quest->play_2, quest->play_2_trigger,
             quest->play_3, quest->play_3_trigger, quest->play_4, quest->play_4_trigger,
             quest->play_5, quest->play_5_trigger, quest->play_6, quest->play_6_trigger,
             quest->play_7, quest->play_7_trigger, quest->play_8, quest->play_8_trigger,
             quest->play_9, quest->play_9_trigger, quest->play_10, quest->play_10_trigger,
             hex_quest_50_ext, hex_quest_60_ext, hex_quest_etc_ext, hex_quest_60_ext_2nd);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacQuest* quest) {
    char query[1024];
    char* values[35];
    DBQueryResult qresult;
    int ret;

    if (!manager || !quest) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, "
             "HEX(quest_10), HEX(quest_15), HEX(quest_20), HEX(quest_30), HEX(quest_40), HEX(quest_40_ext), "
             "HEX(quest_50), HEX(quest_60), HEX(quest_70), HEX(quest_etc), "
             "play_1, play_1_trigger, play_2, play_2_trigger, play_3, play_3_trigger, "
             "play_4, play_4_trigger, play_5, play_5_trigger, play_6, play_6_trigger, "
             "play_7, play_7_trigger, play_8, play_8_trigger, play_9, play_9_trigger, "
             "play_10, play_10_trigger, "
             "HEX(quest_50_ext), HEX(quest_60_ext), HEX(quest_etc_ext), HEX(quest_60_ext_2nd) "
             "FROM charac_quest WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    if (DBQueryResult_FetchRow(&qresult, values) <= 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    /* Extract data */
    quest->charac_no = atoi(values[0]);

    /* Convert hex strings back to binary */
    hex_to_binary(values[1], quest->quest_10, QUEST_DATA_SIZE);
    hex_to_binary(values[2], quest->quest_15, QUEST_DATA_SIZE);
    hex_to_binary(values[3], quest->quest_20, QUEST_DATA_SIZE);
    hex_to_binary(values[4], quest->quest_30, QUEST_DATA_SIZE);
    hex_to_binary(values[5], quest->quest_40, QUEST_DATA_SIZE);
    hex_to_binary(values[6], quest->quest_40_ext, QUEST_DATA_SIZE);
    hex_to_binary(values[7], quest->quest_50, QUEST_DATA_SIZE);
    hex_to_binary(values[8], quest->quest_60, QUEST_DATA_SIZE);
    hex_to_binary(values[9], quest->quest_70, QUEST_DATA_SIZE);
    hex_to_binary(values[10], quest->quest_etc, QUEST_DATA_SIZE);

    /* Extract play slots */
    quest->play_1 = (unsigned short)atoi(values[11]);
    quest->play_1_trigger = atoi(values[12]);
    quest->play_2 = (unsigned short)atoi(values[13]);
    quest->play_2_trigger = atoi(values[14]);
    quest->play_3 = (unsigned short)atoi(values[15]);
    quest->play_3_trigger = atoi(values[16]);
    quest->play_4 = (unsigned short)atoi(values[17]);
    quest->play_4_trigger = atoi(values[18]);
    quest->play_5 = (unsigned short)atoi(values[19]);
    quest->play_5_trigger = atoi(values[20]);
    quest->play_6 = (unsigned short)atoi(values[21]);
    quest->play_6_trigger = atoi(values[22]);
    quest->play_7 = (unsigned short)atoi(values[23]);
    quest->play_7_trigger = atoi(values[24]);
    quest->play_8 = (unsigned short)atoi(values[25]);
    quest->play_8_trigger = atoi(values[26]);
    quest->play_9 = (unsigned short)atoi(values[27]);
    quest->play_9_trigger = atoi(values[28]);
    quest->play_10 = (unsigned short)atoi(values[29]);
    quest->play_10_trigger = atoi(values[30]);

    /* Extract extended data */
    hex_to_binary(values[31], quest->quest_50_ext, QUEST_DATA_SIZE);
    hex_to_binary(values[32], quest->quest_60_ext, QUEST_DATA_SIZE);
    hex_to_binary(values[33], quest->quest_etc_ext, QUEST_DATA_SIZE);
    hex_to_binary(values[34], quest->quest_60_ext_2nd, QUEST_DATA_SIZE);

    DBQueryResult_Free(&qresult);
    return 0;
}

int CharacQuest_Update(DBConnectionManager* manager, CharacQuest* quest) {
    char query[8192];
    char hex_quest_10[129], hex_quest_15[129], hex_quest_20[129];
    char hex_quest_30[129], hex_quest_40[129], hex_quest_40_ext[129];
    char hex_quest_50[129], hex_quest_60[129], hex_quest_70[129], hex_quest_etc[129];
    char hex_quest_50_ext[129], hex_quest_60_ext[129];
    char hex_quest_etc_ext[129], hex_quest_60_ext_2nd[129];
    DBQueryResult qresult;
    int ret;

    if (!manager || !quest) return -1;

    /* Convert binary data to hex strings */
    binary_to_hex(quest->quest_10, QUEST_DATA_SIZE, hex_quest_10);
    binary_to_hex(quest->quest_15, QUEST_DATA_SIZE, hex_quest_15);
    binary_to_hex(quest->quest_20, QUEST_DATA_SIZE, hex_quest_20);
    binary_to_hex(quest->quest_30, QUEST_DATA_SIZE, hex_quest_30);
    binary_to_hex(quest->quest_40, QUEST_DATA_SIZE, hex_quest_40);
    binary_to_hex(quest->quest_40_ext, QUEST_DATA_SIZE, hex_quest_40_ext);
    binary_to_hex(quest->quest_50, QUEST_DATA_SIZE, hex_quest_50);
    binary_to_hex(quest->quest_60, QUEST_DATA_SIZE, hex_quest_60);
    binary_to_hex(quest->quest_70, QUEST_DATA_SIZE, hex_quest_70);
    binary_to_hex(quest->quest_etc, QUEST_DATA_SIZE, hex_quest_etc);
    binary_to_hex(quest->quest_50_ext, QUEST_DATA_SIZE, hex_quest_50_ext);
    binary_to_hex(quest->quest_60_ext, QUEST_DATA_SIZE, hex_quest_60_ext);
    binary_to_hex(quest->quest_etc_ext, QUEST_DATA_SIZE, hex_quest_etc_ext);
    binary_to_hex(quest->quest_60_ext_2nd, QUEST_DATA_SIZE, hex_quest_60_ext_2nd);

    snprintf(query, sizeof(query),
             "UPDATE charac_quest SET "
             "quest_10=UNHEX('%s'), quest_15=UNHEX('%s'), quest_20=UNHEX('%s'), "
             "quest_30=UNHEX('%s'), quest_40=UNHEX('%s'), quest_40_ext=UNHEX('%s'), "
             "quest_50=UNHEX('%s'), quest_60=UNHEX('%s'), quest_70=UNHEX('%s'), quest_etc=UNHEX('%s'), "
             "play_1=%u, play_1_trigger=%d, play_2=%u, play_2_trigger=%d, "
             "play_3=%u, play_3_trigger=%d, play_4=%u, play_4_trigger=%d, "
             "play_5=%u, play_5_trigger=%d, play_6=%u, play_6_trigger=%d, "
             "play_7=%u, play_7_trigger=%d, play_8=%u, play_8_trigger=%d, "
             "play_9=%u, play_9_trigger=%d, play_10=%u, play_10_trigger=%d, "
             "quest_50_ext=UNHEX('%s'), quest_60_ext=UNHEX('%s'), "
             "quest_etc_ext=UNHEX('%s'), quest_60_ext_2nd=UNHEX('%s') "
             "WHERE charac_no=%d",
             hex_quest_10, hex_quest_15, hex_quest_20, hex_quest_30, hex_quest_40, hex_quest_40_ext,
             hex_quest_50, hex_quest_60, hex_quest_70, hex_quest_etc,
             quest->play_1, quest->play_1_trigger, quest->play_2, quest->play_2_trigger,
             quest->play_3, quest->play_3_trigger, quest->play_4, quest->play_4_trigger,
             quest->play_5, quest->play_5_trigger, quest->play_6, quest->play_6_trigger,
             quest->play_7, quest->play_7_trigger, quest->play_8, quest->play_8_trigger,
             quest->play_9, quest->play_9_trigger, quest->play_10, quest->play_10_trigger,
             hex_quest_50_ext, hex_quest_60_ext, hex_quest_etc_ext, hex_quest_60_ext_2nd,
             quest->charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_Delete(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM charac_quest WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_Exists(DBConnectionManager* manager, int charac_no) {
    char query[256];
    char* values[1];
    DBQueryResult qresult;
    int ret;
    int exists = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_quest WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret > 0 && DBQueryResult_FetchRow(&qresult, values) > 0) {
        exists = atoi(values[0]) > 0 ? 1 : 0;
    }

    DBQueryResult_Free(&qresult);
    return exists;
}

int CharacQuest_UpdateCategory(DBConnectionManager* manager, int charac_no,
                                const char* category, const unsigned char* data) {
    char query[512];
    char hex_data[129];
    DBQueryResult qresult;
    int ret;

    if (!manager || !category || !data) return -1;

    binary_to_hex(data, QUEST_DATA_SIZE, hex_data);

    snprintf(query, sizeof(query),
             "UPDATE charac_quest SET %s=UNHEX('%s') WHERE charac_no=%d",
             category, hex_data, charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_UpdatePlaySlot(DBConnectionManager* manager, int charac_no,
                                int slot_num, unsigned short value, int trigger) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager || slot_num < 1 || slot_num > PLAY_SLOT_COUNT) return -1;

    snprintf(query, sizeof(query),
             "UPDATE charac_quest SET play_%d=%u, play_%d_trigger=%d WHERE charac_no=%d",
             slot_num, value, slot_num, trigger, charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_GetTotalCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult qresult;
    int ret;
    int count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_quest");

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret > 0 && DBQueryResult_FetchRow(&qresult, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int CharacQuest_ClearAll(DBConnectionManager* manager, int charac_no) {
    char query[2048];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    /* Set all quest data to zeros and all play slots to 0 */
    snprintf(query, sizeof(query),
             "UPDATE charac_quest SET "
             "quest_10=UNHEX(REPEAT('00', 64)), quest_15=UNHEX(REPEAT('00', 64)), "
             "quest_20=UNHEX(REPEAT('00', 64)), quest_30=UNHEX(REPEAT('00', 64)), "
             "quest_40=UNHEX(REPEAT('00', 64)), quest_40_ext=UNHEX(REPEAT('00', 64)), "
             "quest_50=UNHEX(REPEAT('00', 64)), quest_60=UNHEX(REPEAT('00', 64)), "
             "quest_70=UNHEX(REPEAT('00', 64)), quest_etc=UNHEX(REPEAT('00', 64)), "
             "play_1=0, play_1_trigger=0, play_2=0, play_2_trigger=0, "
             "play_3=0, play_3_trigger=0, play_4=0, play_4_trigger=0, "
             "play_5=0, play_5_trigger=0, play_6=0, play_6_trigger=0, "
             "play_7=0, play_7_trigger=0, play_8=0, play_8_trigger=0, "
             "play_9=0, play_9_trigger=0, play_10=0, play_10_trigger=0, "
             "quest_50_ext=UNHEX(REPEAT('00', 64)), quest_60_ext=UNHEX(REPEAT('00', 64)), "
             "quest_etc_ext=UNHEX(REPEAT('00', 64)), quest_60_ext_2nd=UNHEX(REPEAT('00', 64)) "
             "WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);

    return (ret >= 0) ? 0 : -1;
}

int CharacQuest_GetByPlayValue(DBConnectionManager* manager, int slot_num,
                                unsigned short value, CharacQuest* results, int max_count) {
    char query[512];
    char* values[35];
    DBQueryResult qresult;
    int ret;
    int i, count;

    if (!manager || !results || slot_num < 1 || slot_num > PLAY_SLOT_COUNT) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, "
             "HEX(quest_10), HEX(quest_15), HEX(quest_20), HEX(quest_30), HEX(quest_40), HEX(quest_40_ext), "
             "HEX(quest_50), HEX(quest_60), HEX(quest_70), HEX(quest_etc), "
             "play_1, play_1_trigger, play_2, play_2_trigger, play_3, play_3_trigger, "
             "play_4, play_4_trigger, play_5, play_5_trigger, play_6, play_6_trigger, "
             "play_7, play_7_trigger, play_8, play_8_trigger, play_9, play_9_trigger, "
             "play_10, play_10_trigger, "
             "HEX(quest_50_ext), HEX(quest_60_ext), HEX(quest_etc_ext), HEX(quest_60_ext_2nd) "
             "FROM charac_quest WHERE play_%d=%u LIMIT %d",
             slot_num, value, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret <= 0) {
        DBQueryResult_Free(&qresult);
        return 0;
    }

    count = 0;
    for (i = 0; i < max_count; i++) {
        if (DBQueryResult_FetchRow(&qresult, values) <= 0) {
            break;
        }

        results[i].charac_no = atoi(values[0]);

        /* Convert hex strings back to binary */
        hex_to_binary(values[1], results[i].quest_10, QUEST_DATA_SIZE);
        hex_to_binary(values[2], results[i].quest_15, QUEST_DATA_SIZE);
        hex_to_binary(values[3], results[i].quest_20, QUEST_DATA_SIZE);
        hex_to_binary(values[4], results[i].quest_30, QUEST_DATA_SIZE);
        hex_to_binary(values[5], results[i].quest_40, QUEST_DATA_SIZE);
        hex_to_binary(values[6], results[i].quest_40_ext, QUEST_DATA_SIZE);
        hex_to_binary(values[7], results[i].quest_50, QUEST_DATA_SIZE);
        hex_to_binary(values[8], results[i].quest_60, QUEST_DATA_SIZE);
        hex_to_binary(values[9], results[i].quest_70, QUEST_DATA_SIZE);
        hex_to_binary(values[10], results[i].quest_etc, QUEST_DATA_SIZE);

        /* Extract play slots */
        results[i].play_1 = (unsigned short)atoi(values[11]);
        results[i].play_1_trigger = atoi(values[12]);
        results[i].play_2 = (unsigned short)atoi(values[13]);
        results[i].play_2_trigger = atoi(values[14]);
        results[i].play_3 = (unsigned short)atoi(values[15]);
        results[i].play_3_trigger = atoi(values[16]);
        results[i].play_4 = (unsigned short)atoi(values[17]);
        results[i].play_4_trigger = atoi(values[18]);
        results[i].play_5 = (unsigned short)atoi(values[19]);
        results[i].play_5_trigger = atoi(values[20]);
        results[i].play_6 = (unsigned short)atoi(values[21]);
        results[i].play_6_trigger = atoi(values[22]);
        results[i].play_7 = (unsigned short)atoi(values[23]);
        results[i].play_7_trigger = atoi(values[24]);
        results[i].play_8 = (unsigned short)atoi(values[25]);
        results[i].play_8_trigger = atoi(values[26]);
        results[i].play_9 = (unsigned short)atoi(values[27]);
        results[i].play_9_trigger = atoi(values[28]);
        results[i].play_10 = (unsigned short)atoi(values[29]);
        results[i].play_10_trigger = atoi(values[30]);

        /* Extract extended data */
        hex_to_binary(values[31], results[i].quest_50_ext, QUEST_DATA_SIZE);
        hex_to_binary(values[32], results[i].quest_60_ext, QUEST_DATA_SIZE);
        hex_to_binary(values[33], results[i].quest_etc_ext, QUEST_DATA_SIZE);
        hex_to_binary(values[34], results[i].quest_60_ext_2nd, QUEST_DATA_SIZE);

        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}
