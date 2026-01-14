/**
 * @file pvp_result_orm.c
 * @brief PVP Result ORM Implementation
 */

#include "pvp_result_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void populate_pvp_result(PvpResult* result, char** values) {
    if (!result || !values) return;

    result->charac_no = atoi(values[0]);
    result->win = atoi(values[1]);
    result->lose = atoi(values[2]);
    result->pvp_point = atoi(values[3]);
    result->pvp_grade = atoi(values[4]);
    result->pvp_grade_ext = (unsigned char)atoi(values[5]);
    result->avg_kill_count = atoi(values[6]);
    result->avg_buf_count = atoi(values[7]);
    result->avg_debuf_count = atoi(values[8]);
    result->avg_heal_count = atoi(values[9]);
    result->avg_counter_count = atoi(values[10]);
    result->avg_back_atk_count = atoi(values[11]);
    result->avg_union_hit_count = atoi(values[12]);
    result->avg_overkill_count = atoi(values[13]);
    result->avg_aerial_count = atoi(values[14]);
    result->avg_combo_count = atoi(values[15]);
    result->avg_attacked_count = atoi(values[16]);
    result->avg_deal_damage = atoi(values[17]);
    result->avg_technic = atoi(values[18]);
    result->avg_style = atoi(values[19]);
    result->avg_hit_penalty = atoi(values[20]);
    result->pvp_count = atoi(values[21]);
    result->win_point = atoi(values[22]);

    strncpy(result->last_play_time, values[23] ? values[23] : "0000-00-00 00:00:00", sizeof(result->last_play_time) - 1);
    result->last_play_time[sizeof(result->last_play_time) - 1] = '\0';

    result->play_count = (unsigned int)atoi(values[24]);
    result->play_time = (unsigned int)atoi(values[25]);

    strncpy(result->pvp_grade_ext_update_time, values[26] ? values[26] : "0000-00-00 00:00:00",
            sizeof(result->pvp_grade_ext_update_time) - 1);
    result->pvp_grade_ext_update_time[sizeof(result->pvp_grade_ext_update_time) - 1] = '\0';
}

int PvpResult_Create(DBConnectionManager* manager, PvpResult* result) {
    char query[2048];
    DBQueryResult qresult;
    int ret;

    if (!manager || !result) return -1;

    snprintf(query, sizeof(query),
             "INSERT INTO pvp_result ("
             "charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time"
             ") VALUES ("
             "%d, %d, %d, %d, %d, %u, "
             "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
             "%d, %d, %d, '%s', %u, %u, '%s')",
             result->charac_no, result->win, result->lose, result->pvp_point,
             result->pvp_grade, result->pvp_grade_ext,
             result->avg_kill_count, result->avg_buf_count, result->avg_debuf_count,
             result->avg_heal_count, result->avg_counter_count, result->avg_back_atk_count,
             result->avg_union_hit_count, result->avg_overkill_count, result->avg_aerial_count,
             result->avg_combo_count, result->avg_attacked_count, result->avg_deal_damage,
             result->avg_technic, result->avg_style, result->avg_hit_penalty,
             result->pvp_count, result->win_point, result->last_play_time,
             result->play_count, result->play_time, result->pvp_grade_ext_update_time);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_GetByCharacNo(DBConnectionManager* manager, int charac_no, PvpResult* result) {
    char query[1024];
    char* values[27];
    DBQueryResult qresult;
    int ret;

    if (!manager || !result) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result WHERE charac_no=%d", charac_no);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret > 0 && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(result, values);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&qresult);
    return ret;
}

int PvpResult_Update(DBConnectionManager* manager, PvpResult* result) {
    char query[2048];
    DBQueryResult qresult;
    int ret;

    if (!manager || !result) return -1;

    snprintf(query, sizeof(query),
             "UPDATE pvp_result SET "
             "win=%d, lose=%d, pvp_point=%d, pvp_grade=%d, pvp_grade_ext=%u, "
             "avg_kill_count=%d, avg_buf_count=%d, avg_debuf_count=%d, "
             "avg_heal_count=%d, avg_counter_count=%d, avg_back_atk_count=%d, "
             "avg_union_hit_count=%d, avg_overkill_count=%d, avg_aerial_count=%d, "
             "avg_combo_count=%d, avg_attacked_count=%d, avg_deal_damage=%d, "
             "avg_technic=%d, avg_style=%d, avg_hit_penalty=%d, "
             "pvp_count=%d, win_point=%d, last_play_time='%s', "
             "play_count=%u, play_time=%u, pvp_grade_ext_update_time='%s' "
             "WHERE charac_no=%d",
             result->win, result->lose, result->pvp_point, result->pvp_grade,
             result->pvp_grade_ext,
             result->avg_kill_count, result->avg_buf_count, result->avg_debuf_count,
             result->avg_heal_count, result->avg_counter_count, result->avg_back_atk_count,
             result->avg_union_hit_count, result->avg_overkill_count, result->avg_aerial_count,
             result->avg_combo_count, result->avg_attacked_count, result->avg_deal_damage,
             result->avg_technic, result->avg_style, result->avg_hit_penalty,
             result->pvp_count, result->win_point, result->last_play_time,
             result->play_count, result->play_time, result->pvp_grade_ext_update_time,
             result->charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_Delete(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM pvp_result WHERE charac_no=%d", charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_Exists(DBConnectionManager* manager, int charac_no) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pvp_result WHERE charac_no=%d", charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        ret = atoi(values[0]);
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int PvpResult_RecordMatch(DBConnectionManager* manager, int charac_no, int is_win, int points) {
    char query[512];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    if (is_win) {
        snprintf(query, sizeof(query),
                 "UPDATE pvp_result SET "
                 "win=win+1, pvp_point=pvp_point+%d, win_point=win_point+%d, "
                 "pvp_count=pvp_count+1, last_play_time=NOW() "
                 "WHERE charac_no=%d", points, points, charac_no);
    } else {
        snprintf(query, sizeof(query),
                 "UPDATE pvp_result SET "
                 "lose=lose+1, pvp_point=pvp_point-%d, "
                 "pvp_count=pvp_count+1, last_play_time=NOW() "
                 "WHERE charac_no=%d", points, charac_no);
    }    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_UpdateGrade(DBConnectionManager* manager, int charac_no, int grade, unsigned char grade_ext) {
    char query[512];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE pvp_result SET "
             "pvp_grade=%d, pvp_grade_ext=%u, pvp_grade_ext_update_time=NOW() "
             "WHERE charac_no=%d", grade, grade_ext, charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_UpdateAverages(DBConnectionManager* manager, int charac_no, PvpResult* result) {
    char query[1024];
    DBQueryResult qresult;
    int ret;

    if (!manager || !result) return -1;

    snprintf(query, sizeof(query),
             "UPDATE pvp_result SET "
             "avg_kill_count=%d, avg_buf_count=%d, avg_debuf_count=%d, "
             "avg_heal_count=%d, avg_counter_count=%d, avg_back_atk_count=%d, "
             "avg_union_hit_count=%d, avg_overkill_count=%d, avg_aerial_count=%d, "
             "avg_combo_count=%d, avg_attacked_count=%d, avg_deal_damage=%d, "
             "avg_technic=%d, avg_style=%d, avg_hit_penalty=%d "
             "WHERE charac_no=%d",
             result->avg_kill_count, result->avg_buf_count, result->avg_debuf_count,
             result->avg_heal_count, result->avg_counter_count, result->avg_back_atk_count,
             result->avg_union_hit_count, result->avg_overkill_count, result->avg_aerial_count,
             result->avg_combo_count, result->avg_attacked_count, result->avg_deal_damage,
             result->avg_technic, result->avg_style, result->avg_hit_penalty, charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_GetTopByPoints(DBConnectionManager* manager, PvpResult* results, int max_count) {
    char query[512];
    char* values[27];
    DBQueryResult qresult;
    int ret, count = 0;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result ORDER BY pvp_point DESC LIMIT %d", max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(&results[count], values);
        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int PvpResult_GetTopByWinRate(DBConnectionManager* manager, int min_matches, PvpResult* results, int max_count) {
    char query[512];
    char* values[27];
    DBQueryResult qresult;
    int ret, count = 0;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result WHERE (win+lose)>=%d "
             "ORDER BY (win*100/(win+lose)) DESC LIMIT %d", min_matches, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(&results[count], values);
        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int PvpResult_GetByGrade(DBConnectionManager* manager, int grade, PvpResult* results, int max_count) {
    char query[512];
    char* values[27];
    DBQueryResult qresult;
    int ret, count = 0;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result WHERE pvp_grade=%d LIMIT %d", grade, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(&results[count], values);
        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int PvpResult_GetTotalCount(DBConnectionManager* manager) {
    char query[128];
    char* values[1];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM pvp_result");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        ret = atoi(values[0]);
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int PvpResult_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE pvp_result SET last_play_time=NOW() WHERE charac_no=%d", charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_IncrementPlayCount(DBConnectionManager* manager, int charac_no, unsigned int play_time_seconds) {
    char query[256];
    DBQueryResult qresult;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE pvp_result SET "
             "play_count=play_count+1, play_time=play_time+%u, last_play_time=NOW() "
             "WHERE charac_no=%d", play_time_seconds, charac_no);    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);
    DBQueryResult_Free(&qresult);
    return (ret >= 0) ? 0 : -1;
}

int PvpResult_GetHighWinRate(DBConnectionManager* manager, int min_win_rate, int min_matches,
                             PvpResult* results, int max_count) {
    char query[512];
    char* values[27];
    DBQueryResult qresult;
    int ret, count = 0;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result WHERE (win+lose)>=%d "
             "AND (win*100/(win+lose))>=%d LIMIT %d",
             min_matches, min_win_rate, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(&results[count], values);
        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}

int PvpResult_GetRecentlyActive(DBConnectionManager* manager, int days, PvpResult* results, int max_count) {
    char query[512];
    char* values[27];
    DBQueryResult qresult;
    int ret, count = 0;

    if (!manager || !results || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT charac_no, win, lose, pvp_point, pvp_grade, pvp_grade_ext, "
             "avg_kill_count, avg_buf_count, avg_debuf_count, avg_heal_count, "
             "avg_counter_count, avg_back_atk_count, avg_union_hit_count, "
             "avg_overkill_count, avg_aerial_count, avg_combo_count, "
             "avg_attacked_count, avg_deal_damage, avg_technic, avg_style, "
             "avg_hit_penalty, pvp_count, win_point, last_play_time, "
             "play_count, play_time, pvp_grade_ext_update_time "
             "FROM pvp_result WHERE last_play_time >= DATE_SUB(NOW(), INTERVAL %d DAY) "
             "ORDER BY last_play_time DESC LIMIT %d", days, max_count);

    memset(&qresult, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &qresult);

    if (ret < 0) {
        DBQueryResult_Free(&qresult);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&qresult, values) > 0) {
        populate_pvp_result(&results[count], values);
        count++;
    }

    DBQueryResult_Free(&qresult);
    return count;
}
