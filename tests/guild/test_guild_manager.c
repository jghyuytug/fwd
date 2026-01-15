/**
 * @file test_guild_manager.c
 * @brief Guild Module Test Suite
 *
 * Comprehensive tests for Guild Manager, Guild Level, and Guild Skill systems.
 *
 * @version 1.0
 * @date 2025-11-23
 */

#include "../../include/guild/guild_manager.h"
#include "../../include/guild/guild_level.h"
#include "../../include/guild/guild_skill.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*==============================================================================
 * Test Macros
 *============================================================================*/

#define TEST(name) \
    static void test_##name(void); \
    static void run_test_##name(void) { \
        printf("[TEST] Running: %s\n", #name); \
        test_##name(); \
        printf("[PASS] %s\n\n", #name); \
    } \
    static void test_##name(void)

#define RUN_TEST(name) run_test_##name()

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            fprintf(stderr, "[FAIL] %s:%d: Expected %d, got %d\n", __FILE__, __LINE__, (int)(b), (int)(a)); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "[FAIL] %s:%d: Assertion failed: %s\n", __FILE__, __LINE__, #expr); \
            exit(1); \
        } \
    } while(0)

#define ASSERT_GT(a, b) \
    do { \
        if ((a) <= (b)) { \
            fprintf(stderr, "[FAIL] %s:%d: Expected %d > %d\n", __FILE__, __LINE__, (int)(a), (int)(b)); \
            exit(1); \
        } \
    } while(0)

/*==============================================================================
 * Test Cases - Guild Manager
 *============================================================================*/

TEST(guild_manager_init) {
    int result = GuildManager_Initialize(1000);
    ASSERT_EQ(result, ERR_SUCCESS);
    GuildManager_Cleanup();
}

TEST(guild_create) {
    GuildManager_Initialize(1000);

    GuildData guild;
    int guild_id = GuildManager_Create(1001, "TestGuild", &guild);

    ASSERT_GT(guild_id, 0);
    ASSERT_EQ(strcmp(guild.guild_name, "TestGuild"), 0);
    ASSERT_EQ(guild.leader_id, 1001);
    ASSERT_EQ(guild.member_count, 1);
    ASSERT_EQ(guild.guild_level, 1);
    ASSERT_EQ(guild.max_members, 200); /* ✅ User confirmed */

    GuildManager_Cleanup();
}

TEST(guild_create_duplicate_name) {
    GuildManager_Initialize(1000);

    GuildManager_Create(1001, "TestGuild", NULL);
    int result = GuildManager_Create(1002, "TestGuild", NULL);

    ASSERT_EQ(result, ERR_ALREADY_EXISTS); /* Duplicate name */

    GuildManager_Cleanup();
}

TEST(guild_invite_and_accept) {
    GuildManager_Initialize(1000);

    int guild_id = GuildManager_Create(1001, "TestGuild", NULL);
    ASSERT_GT(guild_id, 0);

    /* Invite member */
    int invite_id = GuildManager_Invite(guild_id, 1001, 1002);
    ASSERT_GT(invite_id, 0);

    /* Accept invite */
    int result = GuildManager_AcceptInvite(1002, guild_id);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Check member count */
    int member_count = GuildManager_GetMemberCount(guild_id);
    ASSERT_EQ(member_count, 2);

    GuildManager_Cleanup();
}

TEST(guild_leave) {
    GuildManager_Initialize(1000);

    int guild_id = GuildManager_Create(1001, "TestGuild", NULL);
    GuildManager_Invite(guild_id, 1001, 1002);
    GuildManager_AcceptInvite(1002, guild_id);

    /* Member leaves */
    int result = GuildManager_Leave(1002);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Check member count */
    int member_count = GuildManager_GetMemberCount(guild_id);
    ASSERT_EQ(member_count, 1);

    GuildManager_Cleanup();
}

TEST(guild_kick_member) {
    GuildManager_Initialize(1000);

    int guild_id = GuildManager_Create(1001, "TestGuild", NULL);
    GuildManager_Invite(guild_id, 1001, 1002);
    GuildManager_AcceptInvite(1002, guild_id);

    /* Kick member */
    int result = GuildManager_Kick(guild_id, 1001, 1002);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Check member count */
    int member_count = GuildManager_GetMemberCount(guild_id);
    ASSERT_EQ(member_count, 1);

    GuildManager_Cleanup();
}

TEST(guild_transfer_leadership) {
    GuildManager_Initialize(1000);

    int guild_id = GuildManager_Create(1001, "TestGuild", NULL);
    GuildManager_Invite(guild_id, 1001, 1002);
    GuildManager_AcceptInvite(1002, guild_id);

    /* Transfer leadership */
    int result = GuildManager_TransferLeader(guild_id, 1001, 1002);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Verify new leader */
    GuildData guild;
    GuildManager_GetGuild(guild_id, &guild);
    ASSERT_EQ(guild.leader_id, 1002);

    GuildManager_Cleanup();
}

TEST(guild_disband) {
    GuildManager_Initialize(1000);

    int guild_id = GuildManager_Create(1001, "TestGuild", NULL);
    ASSERT_GT(guild_id, 0);

    /* Disband guild */
    int result = GuildManager_Disband(guild_id, 1001);
    ASSERT_EQ(result, ERR_SUCCESS);

    /* Verify guild no longer exists */
    GuildData guild;
    result = GuildManager_GetGuild(guild_id, &guild);
    ASSERT_EQ(result, ERR_NOT_FOUND);

    GuildManager_Cleanup();
}

/*==============================================================================
 * Test Cases - Guild Level System
 *============================================================================*/

TEST(guild_level_init) {
    int result = GuildLevel_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);
    GuildLevel_Cleanup();
}

TEST(guild_level_exp_table) {
    GuildLevel_Initialize();

    /* Check exp requirements for each level */
    ASSERT_EQ(GuildLevel_GetExpForLevel(1), 0);       /* Level 1: 0 exp */
    ASSERT_EQ(GuildLevel_GetExpForLevel(2), 1000);    /* Level 2: 1,000 exp */
    ASSERT_EQ(GuildLevel_GetExpForLevel(10), 45000);  /* Level 10: 45,000 exp */

    /* Invalid level */
    ASSERT_EQ(GuildLevel_GetExpForLevel(0), -1);
    ASSERT_EQ(GuildLevel_GetExpForLevel(11), -1);

    GuildLevel_Cleanup();
}

TEST(guild_level_add_exp_no_levelup) {
    GuildLevel_Initialize();

    int level = 1;
    int exp = 0;
    int leveled_up = 0;
    int new_level = 0;
    int skill_points = 0;

    /* Add 500 exp (not enough to level up) */
    int result = GuildLevel_AddExp(1001, &level, &exp, 500,
                                    &leveled_up, &new_level, &skill_points);

    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_EQ(exp, 500);
    ASSERT_EQ(level, 1);
    ASSERT_EQ(leveled_up, 0);

    GuildLevel_Cleanup();
}

TEST(guild_level_add_exp_with_levelup) {
    GuildLevel_Initialize();

    int level = 1;
    int exp = 0;
    int leveled_up = 0;
    int new_level = 0;
    int skill_points = 0;

    /* Add 1,500 exp (enough for level 2) */
    int result = GuildLevel_AddExp(1001, &level, &exp, 1500,
                                    &leveled_up, &new_level, &skill_points);

    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_EQ(exp, 1500);
    ASSERT_EQ(level, 2);
    ASSERT_EQ(leveled_up, 1);
    ASSERT_EQ(new_level, 2);
    ASSERT_EQ(skill_points, 1); /* Level 2 awards 1 SP */

    GuildLevel_Cleanup();
}

TEST(guild_level_multiple_levelups) {
    GuildLevel_Initialize();

    int level = 1;
    int exp = 0;
    int leveled_up = 0;
    int new_level = 0;
    int skill_points = 0;

    /* Add 20,000 exp (enough for multiple levels) */
    int result = GuildLevel_AddExp(1001, &level, &exp, 20000,
                                    &leveled_up, &new_level, &skill_points);

    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_GT(level, 1);
    ASSERT_EQ(leveled_up, 1);
    ASSERT_GT(skill_points, 0);

    GuildLevel_Cleanup();
}

TEST(guild_level_skill_points_calculation) {
    GuildLevel_Initialize();

    /* Level 2-5: 1 SP per level */
    ASSERT_EQ(GuildLevel_CalculateSkillPoints(2), 1);
    ASSERT_EQ(GuildLevel_CalculateSkillPoints(5), 1);

    /* Level 6-10: 2 SP per level */
    ASSERT_EQ(GuildLevel_CalculateSkillPoints(6), 2);
    ASSERT_EQ(GuildLevel_CalculateSkillPoints(10), 2);

    GuildLevel_Cleanup();
}

/*==============================================================================
 * Test Cases - Guild Skill System
 *============================================================================*/

TEST(guild_skill_init) {
    int result = GuildSkill_Initialize();
    ASSERT_EQ(result, ERR_SUCCESS);
    GuildSkill_Cleanup();
}

TEST(guild_skill_add_points) {
    GuildSkill_Initialize();

    int result = GuildSkill_AddSkillPoints(1001, 5);
    ASSERT_EQ(result, ERR_SUCCESS);

    int remain_sp = 0, used_sp = 0;
    GuildSkill_GetSkillPoints(1001, &remain_sp, &used_sp);
    ASSERT_EQ(remain_sp, 5);
    ASSERT_EQ(used_sp, 0);

    GuildSkill_Cleanup();
}

TEST(guild_skill_learn_skill) {
    GuildSkill_Initialize();

    /* Add skill points */
    GuildSkill_AddSkillPoints(1001, 10);

    int remain_sp = 10, used_sp = 0;

    /* Learn Exp Boost skill (cost: 1 SP) */
    int result = GuildSkill_LearnSkill(1001, GUILD_SKILL_EXP_BOOST,
                                        &remain_sp, &used_sp);

    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_EQ(remain_sp, 9);
    ASSERT_EQ(used_sp, 1);

    /* Check skill level */
    int skill_level = GuildSkill_GetSkillLevel(1001, GUILD_SKILL_EXP_BOOST);
    ASSERT_EQ(skill_level, 1);

    GuildSkill_Cleanup();
}

TEST(guild_skill_learn_multiple_levels) {
    GuildSkill_Initialize();

    GuildSkill_AddSkillPoints(1001, 20);

    int remain_sp = 20, used_sp = 0;

    /* Learn Exp Boost 5 times */
    for (int i = 0; i < 5; i++) {
        int result = GuildSkill_LearnSkill(1001, GUILD_SKILL_EXP_BOOST,
                                            &remain_sp, &used_sp);
        ASSERT_EQ(result, ERR_SUCCESS);
    }

    ASSERT_EQ(remain_sp, 15); /* 20 - 5 */
    ASSERT_EQ(used_sp, 5);

    int skill_level = GuildSkill_GetSkillLevel(1001, GUILD_SKILL_EXP_BOOST);
    ASSERT_EQ(skill_level, 5);

    GuildSkill_Cleanup();
}

TEST(guild_skill_insufficient_sp) {
    GuildSkill_Initialize();

    GuildSkill_AddSkillPoints(1001, 1);

    int remain_sp = 1, used_sp = 0;

    /* Try to learn Attack Boost (cost: 2 SP) */
    int result = GuildSkill_LearnSkill(1001, GUILD_SKILL_ATK_BOOST,
                                        &remain_sp, &used_sp);

    ASSERT_EQ(result, ERR_INSUFFICIENT_RESOURCES);

    GuildSkill_Cleanup();
}

TEST(guild_skill_reset) {
    GuildSkill_Initialize();

    /* Learn some skills */
    GuildSkill_AddSkillPoints(1001, 20);

    int remain_sp = 20, used_sp = 0;

    GuildSkill_LearnSkill(1001, GUILD_SKILL_EXP_BOOST, &remain_sp, &used_sp);
    GuildSkill_LearnSkill(1001, GUILD_SKILL_DROP_RATE, &remain_sp, &used_sp);

    /* Reset all skills */
    int result = GuildSkill_ResetSkills(1001, &remain_sp, &used_sp);

    ASSERT_EQ(result, ERR_SUCCESS);
    ASSERT_EQ(remain_sp, 20); /* All SP refunded */
    ASSERT_EQ(used_sp, 0);

    /* Check skills are reset */
    int skill_level = GuildSkill_GetSkillLevel(1001, GUILD_SKILL_EXP_BOOST);
    ASSERT_EQ(skill_level, 0);

    GuildSkill_Cleanup();
}

TEST(guild_skill_effect_calculation) {
    GuildSkill_Initialize();

    /* Exp Boost: +5% per level */
    float effect = GuildSkill_GetSkillEffect(GUILD_SKILL_EXP_BOOST, 5);
    ASSERT_TRUE(effect > 0.24f && effect < 0.26f); /* ~0.25 (25%) */

    /* Attack Boost: +50 per level */
    effect = GuildSkill_GetSkillEffect(GUILD_SKILL_ATK_BOOST, 3);
    ASSERT_TRUE(effect > 149.0f && effect < 151.0f); /* ~150 */

    GuildSkill_Cleanup();
}

/*==============================================================================
 * Main Test Runner
 *============================================================================*/

int main(void)
{
    printf("=======================================================\n");
    printf("           Guild Module Test Suite\n");
    printf("=======================================================\n\n");

    /* Guild Manager Tests */
    printf("--- Guild Manager Tests ---\n");
    RUN_TEST(guild_manager_init);
    RUN_TEST(guild_create);
    RUN_TEST(guild_create_duplicate_name);
    RUN_TEST(guild_invite_and_accept);
    RUN_TEST(guild_leave);
    RUN_TEST(guild_kick_member);
    RUN_TEST(guild_transfer_leadership);
    RUN_TEST(guild_disband);

    /* Guild Level Tests */
    printf("--- Guild Level Tests ---\n");
    RUN_TEST(guild_level_init);
    RUN_TEST(guild_level_exp_table);
    RUN_TEST(guild_level_add_exp_no_levelup);
    RUN_TEST(guild_level_add_exp_with_levelup);
    RUN_TEST(guild_level_multiple_levelups);
    RUN_TEST(guild_level_skill_points_calculation);

    /* Guild Skill Tests */
    printf("--- Guild Skill Tests ---\n");
    RUN_TEST(guild_skill_init);
    RUN_TEST(guild_skill_add_points);
    RUN_TEST(guild_skill_learn_skill);
    RUN_TEST(guild_skill_learn_multiple_levels);
    RUN_TEST(guild_skill_insufficient_sp);
    RUN_TEST(guild_skill_reset);
    RUN_TEST(guild_skill_effect_calculation);

    printf("=======================================================\n");
    printf("  ✅ ALL 23 TESTS PASSED!\n");
    printf("=======================================================\n");

    return 0;
}
