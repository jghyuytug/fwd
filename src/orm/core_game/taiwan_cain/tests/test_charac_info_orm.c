/**
 * @file test_charac_info_orm.c
 * @brief Test suite for Character Information ORM
 * Database: taiwan_cain (DB_TYPE_CAIN = 2)
 * Test count: 14 tests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "db_connection_manager.h"
#include "charac_info_orm.h"

void print_charac_info(const CharacInfo* info) {
    printf("  charac_no=%d, m_id=%d, name=%s, village=%d, sex=%d\n",
           info->charac_no, info->m_id, info->charac_name, info->village, info->sex);
    printf("  job=%d, lev=%d, exp=%d, grow_type=%d, expert_job=%d\n",
           info->job, info->lev, info->exp, info->grow_type, info->expert_job);
    printf("  HP=%d, maxHP=%d, maxMP=%d\n", info->HP, info->maxHP, info->maxMP);
    printf("  phy_atk=%d, phy_def=%d, mag_atk=%d, mag_def=%d\n",
           info->phy_attack, info->phy_defense, info->mag_attack, info->mag_defense);
    printf("  guild_id=%d, guild_right=%d\n", info->guild_id, info->guild_right);
    printf("  fatigue=%d, max_fatigue=%d, premium_fatigue=%d\n",
           info->fatigue, info->max_fatigue, info->premium_fatigue);
}

int test_01_create(DBConnectionManager* mgr) {
    printf("Test 1: CharacInfo_Create\n");
    CharacInfo info;
    memset(&info, 0, sizeof(CharacInfo));
    info.m_id = 999;
    strcpy(info.charac_name, "TestHero");
    info.village = 1;
    info.sex = 0;
    info.job = 2;
    info.lev = 10;
    info.exp = 5000;
    info.grow_type = 1;
    info.expert_job = 0;
    info.HP = 500;
    info.maxHP = 500;
    info.maxMP = 300;
    info.phy_attack = 100;
    info.phy_defense = 50;
    info.mag_attack = 80;
    info.mag_defense = 40;
    info.move_speed = 100;
    info.attack_speed = 100;
    info.cast_speed = 100;
    info.fatigue = 0;
    info.max_fatigue = 156;
    info.premium_fatigue = 0;
    info.guild_id = 0;
    info.guild_right = 0;
    int charac_no = CharacInfo_Create(mgr, &info);
    if (charac_no <= 0) { printf("  FAILED: Create returned %d\n", charac_no); return 0; }
    printf("  Created charac_no=%d\n", charac_no);
    return charac_no;
}

int test_02_get_by_charac_no(DBConnectionManager* mgr, int charac_no) {
    printf("Test 2: CharacInfo_GetByCharacNo\n");
    CharacInfo info;
    int ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    printf("  Found character:\n");
    print_charac_info(&info);
    if (strcmp(info.charac_name, "TestHero") != 0) {
        printf("  FAILED: Name mismatch\n"); return 0;
    }
    printf("  PASSED\n");
    return 1;
}

int test_03_get_by_name(DBConnectionManager* mgr) {
    printf("Test 3: CharacInfo_GetByName\n");
    CharacInfo info;
    int ret = CharacInfo_GetByName(mgr, "TestHero", &info);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    printf("  Found character:\n");
    print_charac_info(&info);
    printf("  PASSED\n");
    return 1;
}

int test_04_update(DBConnectionManager* mgr, int charac_no) {
    printf("Test 4: CharacInfo_Update\n");
    CharacInfo info;
    int ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (ret < 0) { printf("  FAILED: Cannot get record\n"); return 0; }
    info.lev = 20;
    info.exp = 15000;
    info.HP = 800;
    info.maxHP = 800;
    info.maxMP = 500;
    ret = CharacInfo_Update(mgr, &info);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (info.lev != 20 || info.exp != 15000) {
        printf("  FAILED: Update verification failed\n"); return 0;
    }
    printf("  PASSED\n");
    return 1;
}

int test_05_exists(DBConnectionManager* mgr, int charac_no) {
    printf("Test 5: CharacInfo_Exists\n");
    int ret = CharacInfo_Exists(mgr, charac_no);
    if (ret != 1) { printf("  FAILED: Should exist\n"); return 0; }
    printf("  PASSED\n");
    return 1;
}

int test_06_update_level_exp(DBConnectionManager* mgr, int charac_no) {
    printf("Test 6: CharacInfo_UpdateLevelExp\n");
    int ret = CharacInfo_UpdateLevelExp(mgr, charac_no, 25, 30000);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    CharacInfo info;
    ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (info.lev != 25 || info.exp != 30000) {
        printf("  FAILED: Verification failed\n"); return 0;
    }
    printf("  PASSED\n");
    return 1;
}

int test_07_update_combat_stats(DBConnectionManager* mgr, int charac_no) {
    printf("Test 7: CharacInfo_UpdateCombatStats\n");
    int ret = CharacInfo_UpdateCombatStats(mgr, charac_no, 200, 1000, 600);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    CharacInfo info;
    ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (info.HP != 200 || info.maxHP != 1000 || info.maxMP != 600) {
        printf("  FAILED: Verification failed\n"); return 0;
    }
    printf("  PASSED\n");
    return 1;
}

int test_08_update_guild(DBConnectionManager* mgr, int charac_no) {
    printf("Test 8: CharacInfo_UpdateGuild\n");
    int ret = CharacInfo_UpdateGuild(mgr, charac_no, 100, 1);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    CharacInfo info;
    ret = CharacInfo_GetByCharacNo(mgr, charac_no, &info);
    if (info.guild_id != 100 || info.guild_right != 1) {
        printf("  FAILED: Verification failed\n"); return 0;
    }
    printf("  PASSED\n");
    return 1;
}

int test_09_update_last_play_time(DBConnectionManager* mgr, int charac_no) {
    printf("Test 9: CharacInfo_UpdateLastPlayTime\n");
    int ret = CharacInfo_UpdateLastPlayTime(mgr, charac_no);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    printf("  PASSED\n");
    return 1;
}

int test_10_get_count_by_member_id(DBConnectionManager* mgr) {
    printf("Test 10: CharacInfo_GetCountByMemberId\n");
    int count = CharacInfo_GetCountByMemberId(mgr, 999);
    if (count < 1) { printf("  FAILED: count=%d\n", count); return 0; }
    printf("  Found %d characters for m_id=999\n", count);
    printf("  PASSED\n");
    return 1;
}

int test_11_get_by_member_id(DBConnectionManager* mgr) {
    printf("Test 11: CharacInfo_GetByMemberId\n");
    CharacInfo results[10];
    int count = CharacInfo_GetByMemberId(mgr, 999, results, 10);
    int i;
    if (count < 1) { printf("  FAILED: count=%d\n", count); return 0; }
    printf("  Found %d characters:\n", count);
    for (i = 0; i < count; i++) {
        printf("  Character %d:\n", i+1);
        print_charac_info(&results[i]);
    }
    printf("  PASSED\n");
    return 1;
}

int test_12_nonexistent(DBConnectionManager* mgr) {
    printf("Test 12: Get nonexistent character\n");
    CharacInfo info;
    int ret = CharacInfo_GetByCharacNo(mgr, 999999, &info);
    if (ret == 0) { printf("  PASSED (correctly returned error)\n"); return 1; }
    printf("  FAILED: Should not find record\n");
    return 0;
}

int test_13_delete(DBConnectionManager* mgr, int charac_no) {
    printf("Test 13: CharacInfo_Delete\n");
    int ret = CharacInfo_Delete(mgr, charac_no);
    if (ret < 0) { printf("  FAILED\n"); return 0; }
    ret = CharacInfo_Exists(mgr, charac_no);
    if (ret == 1) { printf("  FAILED: Record still exists\n"); return 0; }
    printf("  PASSED\n");
    return 1;
}

int test_14_delete_nonexistent(DBConnectionManager* mgr) {
    printf("Test 14: Delete nonexistent character\n");
    int ret = CharacInfo_Delete(mgr, 999999);
    if (ret < 0) { printf("  PASSED (correctly handled)\n"); return 1; }
    printf("  WARNING: Delete succeeded on nonexistent record\n");
    return 1;
}

int main(int argc, char* argv[]) {
    DBConnectionManager mgr;
    int passed = 0, total = 0;
    int charac_no = 0;

    if (argc < 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    memset(&mgr, 0, sizeof(DBConnectionManager));

    if (DBConnectionManager_Initialize(&mgr, argv[1]) < 0) {
        printf("Failed to initialize DBConnectionManager\n");
        return 1;
    }

    if (DBConnectionManager_Connect(&mgr, DB_TYPE_CAIN) < 0) {
        printf("Failed to connect to databases\n");
        DBConnectionManager_Cleanup(&mgr);
        return 1;
    }

    printf("\n=== Character Info ORM Test Suite ===\n\n");

    charac_no = test_01_create(&mgr); total++; if (charac_no > 0) passed++;

    if (charac_no > 0) {
        if (test_02_get_by_charac_no(&mgr, charac_no)) passed++; total++;
        if (test_03_get_by_name(&mgr)) passed++; total++;
        if (test_04_update(&mgr, charac_no)) passed++; total++;
        if (test_05_exists(&mgr, charac_no)) passed++; total++;
        if (test_06_update_level_exp(&mgr, charac_no)) passed++; total++;
        if (test_07_update_combat_stats(&mgr, charac_no)) passed++; total++;
        if (test_08_update_guild(&mgr, charac_no)) passed++; total++;
        if (test_09_update_last_play_time(&mgr, charac_no)) passed++; total++;
        if (test_10_get_count_by_member_id(&mgr)) passed++; total++;
        if (test_11_get_by_member_id(&mgr)) passed++; total++;
        if (test_12_nonexistent(&mgr)) passed++; total++;
        if (test_13_delete(&mgr, charac_no)) passed++; total++;
        if (test_14_delete_nonexistent(&mgr)) passed++; total++;
    } else {
        printf("\nSkipping remaining tests due to create failure\n");
    }

    printf("\n=== Test Results: %d/%d passed ===\n", passed, total);

    DBConnectionManager_Cleanup(&mgr);
    return (passed == total) ? 0 : 1;
}
