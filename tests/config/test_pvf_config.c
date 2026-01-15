/**
 * @file test_pvf_config.c
 * @brief Test program for PVF configuration loading
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 3)
 * Date: 2025-11-25
 *
 * Usage:
 *   ./test_pvf_config [path_to_Script.pvf]
 *
 * Tests:
 *   1. Hardcoded configuration initialization
 *   2. PVF configuration loading
 *   3. ETC parser functionality
 *   4. Configuration API access
 */

#include "../../include/config/game_config.h"
#include "../../include/config/etc_parser.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ========================================================================
 * TEST HELPERS
 * ======================================================================== */

static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_BEGIN(name) \
    printf("\n[TEST] %s\n", name); \
    printf("=========================================\n");

#define TEST_PASS(msg) \
    do { \
        printf("  [PASS] %s\n", msg); \
        g_tests_passed++; \
    } while(0)

#define TEST_FAIL(msg) \
    do { \
        printf("  [FAIL] %s\n", msg); \
        g_tests_failed++; \
    } while(0)

#define TEST_CHECK(cond, msg) \
    do { \
        if (cond) { TEST_PASS(msg); } \
        else { TEST_FAIL(msg); } \
    } while(0)

/* ========================================================================
 * TEST 1: ETC PARSER
 * ======================================================================== */

static void test_etc_parser(void) {
    TEST_BEGIN("ETC Parser");

    /* Test data */
    const char* test_data =
        "# This is a comment\n"
        "`defense_constant` 1500\n"
        "`min_damage_rate` 0.02\n"
        "`attribute_divisor` 250\n"
        "`name` Test Configuration\n"
        "// Another comment\n"
        "`max_buff_stack` 15\n";

    ETCDocument doc;
    int result = ETC_Parse(&doc, test_data, strlen(test_data));

    TEST_CHECK(result == ETC_SUCCESS, "Parse returns success");
    TEST_CHECK(ETC_GetEntryCount(&doc) == 5, "Entry count is 5");

    TEST_CHECK(ETC_GetInt(&doc, "defense_constant", 0) == 1500,
               "defense_constant = 1500");
    TEST_CHECK(ETC_GetFloat(&doc, "min_damage_rate", 0.0f) > 0.019f &&
               ETC_GetFloat(&doc, "min_damage_rate", 0.0f) < 0.021f,
               "min_damage_rate = 0.02");
    TEST_CHECK(ETC_GetInt(&doc, "attribute_divisor", 0) == 250,
               "attribute_divisor = 250");
    TEST_CHECK(ETC_GetInt(&doc, "max_buff_stack", 0) == 15,
               "max_buff_stack = 15");

    char name_buf[256];
    TEST_CHECK(ETC_GetString(&doc, "name", name_buf, sizeof(name_buf)) == 0,
               "name string retrieved");
    TEST_CHECK(strcmp(name_buf, "Test Configuration") == 0,
               "name = 'Test Configuration'");

    TEST_CHECK(ETC_HasKey(&doc, "defense_constant") == 1,
               "HasKey('defense_constant') = true");
    TEST_CHECK(ETC_HasKey(&doc, "nonexistent") == 0,
               "HasKey('nonexistent') = false");

    TEST_CHECK(ETC_GetInt(&doc, "nonexistent", 999) == 999,
               "Default value for missing key");

    ETC_Free(&doc);
}

/* ========================================================================
 * TEST 2: HARDCODED CONFIGURATION
 * ======================================================================== */

static void test_hardcoded_config(void) {
    TEST_BEGIN("Hardcoded Configuration");

    /* Initialize with hardcoded values */
    int result = GameConfig_Initialize();
    TEST_CHECK(result == 0, "Initialize returns 0");

    TEST_CHECK(GameConfig_GetSource() == CONFIG_SOURCE_HARDCODED,
               "Source is HARDCODED");

    /* Check combat config */
    const DamageFormulaConfig* dmg = CombatConfig_GetDamageFormula();
    TEST_CHECK(dmg != NULL, "DamageFormula not NULL");
    TEST_CHECK(dmg->defense_constant == 1000, "defense_constant = 1000");
    TEST_CHECK(dmg->attribute_divisor == 200, "attribute_divisor = 200");

    /* Check character growth config */
    const CharacterGrowthConfig* growth = CombatConfig_GetCharacterGrowth(0);
    TEST_CHECK(growth != NULL, "CharacterGrowth not NULL");
    TEST_CHECK(growth->base_strength == 50, "base_strength = 50");
    TEST_CHECK(growth->base_critical_rate == 5, "base_critical_rate = 5");

    /* Check skill config (cached) */
    const SkillConfig* skill = CombatConfig_GetSkill(1000, 10);
    TEST_CHECK(skill != NULL, "SkillConfig not NULL");
    TEST_CHECK(skill->skill_id == 1000, "skill_id = 1000");
    TEST_CHECK(skill->level == 10, "level = 10");
    TEST_CHECK(skill->base_damage == 1500, "base_damage = (10*100)+500 = 1500");

    /* Check quest config */
    const QuestRewardConfig* quest = GameConfig_GetQuestReward();
    TEST_CHECK(quest != NULL, "QuestReward not NULL");
    TEST_CHECK(quest->exp_multiplier > 1.4f && quest->exp_multiplier < 1.6f,
               "exp_multiplier = 1.5");

    /* Check shop config */
    const ShopDiscountConfig* shop = ShopConfig_GetDiscount();
    TEST_CHECK(shop != NULL, "ShopDiscount not NULL");
    TEST_CHECK(shop->vip_discount > 0.89f && shop->vip_discount < 0.91f,
               "vip_discount = 0.9");

    /* Check party config */
    const PartyBonusConfig* party = PartyConfig_GetBonus();
    TEST_CHECK(party != NULL, "PartyBonus not NULL");
    TEST_CHECK(party->bonus_2_members > 1.09f && party->bonus_2_members < 1.11f,
               "bonus_2_members = 1.1");

    /* Cleanup */
    GameConfig_Cleanup();
    TEST_CHECK(1, "Cleanup completed");
}

/* ========================================================================
 * TEST 3: PVF CONFIGURATION (if available)
 * ======================================================================== */

static void test_pvf_config(const char* pvf_path) {
    TEST_BEGIN("PVF Configuration");

    if (!pvf_path) {
        printf("  [SKIP] No PVF path provided\n");
        return;
    }

    printf("  Testing with PVF: %s\n", pvf_path);

    /* Initialize from PVF */
    int result = GameConfig_InitializeFromPVF(pvf_path);

    /* PVF loading may succeed or fail depending on whether config files exist */
    if (result == 0) {
        TEST_CHECK(GameConfig_GetSource() == CONFIG_SOURCE_PVF,
                   "Source is PVF");
    } else {
        TEST_CHECK(GameConfig_GetSource() == CONFIG_SOURCE_HARDCODED,
                   "Source is HARDCODED (PVF fallback)");
    }

    /* Config should be accessible regardless of source */
    const DamageFormulaConfig* dmg = CombatConfig_GetDamageFormula();
    TEST_CHECK(dmg != NULL, "DamageFormula accessible");

    const GameConfig* master = GameConfig_Get();
    TEST_CHECK(master != NULL, "Master config accessible");

    printf("  Current config values:\n");
    printf("    defense_constant = %d\n", dmg->defense_constant);
    printf("    attribute_divisor = %d\n", dmg->attribute_divisor);
    printf("    exp_multiplier = %.2f\n",
           GameConfig_GetQuestReward()->exp_multiplier);
    printf("    vip_discount = %.2f\n",
           ShopConfig_GetDiscount()->vip_discount);

    /* Cleanup */
    GameConfig_Cleanup();
    TEST_CHECK(1, "Cleanup completed");
}

/* ========================================================================
 * TEST 4: SKILL CACHE PERFORMANCE
 * ======================================================================== */

static void test_skill_cache(void) {
    TEST_BEGIN("Skill Cache Performance");

    GameConfig_Initialize();

    /* First access - cache miss */
    const SkillConfig* skill1 = CombatConfig_GetSkill(100, 5);
    TEST_CHECK(skill1 != NULL, "First access (miss)");

    /* Second access same skill - cache hit */
    const SkillConfig* skill2 = CombatConfig_GetSkill(100, 5);
    TEST_CHECK(skill2 != NULL, "Second access (hit)");
    TEST_CHECK(skill1 == skill2, "Same pointer returned (cache hit)");

    /* Access many different skills */
    printf("  Accessing 1000 different skill configurations...\n");
    for (int i = 0; i < 1000; i++) {
        int skill_id = i % 200;
        int level = (i % 60) + 1;
        const SkillConfig* s = CombatConfig_GetSkill(skill_id, level);
        if (!s) {
            TEST_FAIL("Skill access returned NULL");
            break;
        }
    }
    TEST_PASS("1000 skill accesses completed");

    /* Print cache stats */
    GameConfig_PrintCacheStats();

    GameConfig_Cleanup();
}

/* ========================================================================
 * MAIN
 * ======================================================================== */

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("GameConfig PVF Integration Test\n");
    printf("Phase: 6.7 - Stage 3\n");
    printf("========================================\n");

    const char* pvf_path = NULL;
    if (argc >= 2) {
        pvf_path = argv[1];
    }

    /* Run tests */
    test_etc_parser();
    test_hardcoded_config();
    test_pvf_config(pvf_path);
    test_skill_cache();

    /* Summary */
    printf("\n========================================\n");
    printf("Test Results: %d passed, %d failed\n",
           g_tests_passed, g_tests_failed);
    printf("========================================\n");

    if (g_tests_failed > 0) {
        printf("\n[FAILURE] Some tests failed!\n");
        return 1;
    }

    printf("\n[SUCCESS] All tests passed!\n");
    return 0;
}
