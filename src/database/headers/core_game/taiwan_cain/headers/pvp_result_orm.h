/**
 * @file pvp_result_orm.h
 * @brief PVP Result ORM Module - Player PVP Statistics Management
 */

#ifndef PVP_RESULT_ORM_H
#define PVP_RESULT_ORM_H

#include "db_connection_manager.h"
#include <time.h>

typedef struct {
    int charac_no;
    int win;
    int lose;
    int pvp_point;
    int pvp_grade;
    unsigned char pvp_grade_ext;
    int avg_kill_count;
    int avg_buf_count;
    int avg_debuf_count;
    int avg_heal_count;
    int avg_counter_count;
    int avg_back_atk_count;
    int avg_union_hit_count;
    int avg_overkill_count;
    int avg_aerial_count;
    int avg_combo_count;
    int avg_attacked_count;
    int avg_deal_damage;
    int avg_technic;
    int avg_style;
    int avg_hit_penalty;
    int pvp_count;
    int win_point;
    char last_play_time[32];
    unsigned int play_count;
    unsigned int play_time;
    char pvp_grade_ext_update_time[32];
} PvpResult;

int PvpResult_Create(DBConnectionManager* manager, PvpResult* result);
int PvpResult_GetByCharacNo(DBConnectionManager* manager, int charac_no, PvpResult* result);
int PvpResult_Update(DBConnectionManager* manager, PvpResult* result);
int PvpResult_Delete(DBConnectionManager* manager, int charac_no);
int PvpResult_Exists(DBConnectionManager* manager, int charac_no);
int PvpResult_RecordMatch(DBConnectionManager* manager, int charac_no, int is_win, int points);
int PvpResult_UpdateGrade(DBConnectionManager* manager, int charac_no, int grade, unsigned char grade_ext);
int PvpResult_UpdateAverages(DBConnectionManager* manager, int charac_no, PvpResult* result);
int PvpResult_GetTopByPoints(DBConnectionManager* manager, PvpResult* results, int max_count);
int PvpResult_GetTopByWinRate(DBConnectionManager* manager, int min_matches, PvpResult* results, int max_count);
int PvpResult_GetByGrade(DBConnectionManager* manager, int grade, PvpResult* results, int max_count);
int PvpResult_GetTotalCount(DBConnectionManager* manager);
int PvpResult_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no);
int PvpResult_IncrementPlayCount(DBConnectionManager* manager, int charac_no, unsigned int play_time_seconds);
int PvpResult_GetHighWinRate(DBConnectionManager* manager, int min_win_rate, int min_matches, PvpResult* results, int max_count);
int PvpResult_GetRecentlyActive(DBConnectionManager* manager, int days, PvpResult* results, int max_count);

#endif /* PVP_RESULT_ORM_H */
