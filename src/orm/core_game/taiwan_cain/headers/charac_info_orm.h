/**
 * @file charac_info_orm.h
 * @brief Character Information ORM Module
 */

#ifndef CHARAC_INFO_ORM_H
#define CHARAC_INFO_ORM_H

#include "db_connection_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int charac_no;
    int m_id;
    char charac_name[20];
    signed char village;
    signed char sex;
    signed char job;
    unsigned char lev;
    int exp;
    unsigned char grow_type;
    signed char expert_job;
    int HP;
    int maxHP;
    int maxMP;
    int phy_attack;
    int phy_defense;
    int mag_attack;
    int mag_defense;
    char element_resist[256];
    char spec_property[256];
    int move_speed;
    int attack_speed;
    int cast_speed;
    int fatigue;
    int max_fatigue;
    int premium_fatigue;
    int guild_id;
    signed char guild_right;
    char create_time[32];
    char last_play_time[32];
    char delete_time[32];
    char stat_point_apply_flag[2];
    int competition_point;
    signed char mercenary_flag;
    int competition_rank_point;
    int party_invite_grade;
    int mercenary_reserve_type;
    signed char mercenary_reserve_state;
    int mercenary_reserve_time;
    char VIP[2];
    int competition_season_point;
    int competition_season_id;
    int competition_season_rank_point;
    signed char competition_season_reward_flag;
    int grow_quest_clear_flag;
    signed char competition_normal;
    signed char competition_reward_flag;
    int competition_reward_time;
    signed char arad_adventurer;
    signed char add_phy_defense;
    signed char add_mag_defense;
} CharacInfo;

int CharacInfo_Create(DBConnectionManager* manager, CharacInfo* info);
int CharacInfo_GetByCharacNo(DBConnectionManager* manager, int charac_no, CharacInfo* info);
int CharacInfo_GetByName(DBConnectionManager* manager, const char* charac_name, CharacInfo* info);
int CharacInfo_Update(DBConnectionManager* manager, CharacInfo* info);
int CharacInfo_Delete(DBConnectionManager* manager, int charac_no);
int CharacInfo_Exists(DBConnectionManager* manager, int charac_no);
int CharacInfo_UpdateLevelExp(DBConnectionManager* manager, int charac_no, unsigned char lev, int exp);
int CharacInfo_UpdateCombatStats(DBConnectionManager* manager, int charac_no, int HP, int maxHP, int maxMP);
int CharacInfo_UpdateGuild(DBConnectionManager* manager, int charac_no, int guild_id, signed char guild_right);
int CharacInfo_UpdateLastPlayTime(DBConnectionManager* manager, int charac_no);
int CharacInfo_GetByMemberId(DBConnectionManager* manager, int m_id, CharacInfo* results, int max_count);
int CharacInfo_GetCountByMemberId(DBConnectionManager* manager, int m_id);

#ifdef __cplusplus
}
#endif

#endif
