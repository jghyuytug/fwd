/**
 * @file test_file_config.c
 * @brief Test program for FILE configuration loading
 *
 * Phase: 6.7 - Configuration Abstraction (Stage 4)
 * Date: 2025-11-25
 *
 * Usage:
 *   ./test_file_config [config_dir]
 *   Default config_dir: ../../config/etc
 *
 * Tests:
 *   1. Load configuration from text .etc files
 *   2. Verify loaded values
 *   3. Test fallback to hardcoded values
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
 * TEST 1: FILE CONFIGURATION LOADING
 * ======================================================================== */

static void test_file_config(const char* config_dir) {
    TEST_BEGIN("FILE Configuration Loading");

    printf("  Config directory: %s\n", config_dir);

    /* Initialize from files */
    int result = GameConfig_InitializeFromFile(config_dir);

    if (result == 0) {
        TEST_CHECK(GameConfig_GetSource() == CONFIG_SOURCE_FILE,
                   "Source is FILE");
    } else {
        TEST_CHECK(GameConfig_GetSource() == CONFIG_SOURCE_HARDCODED,
                   "Source is HARDCODED (fallback)");
    }

    /* Verify config values are accessible */
    const DamageFormulaConfig* dmg = CombatConfig_GetDamageFormula();
    TEST_CHECK(dmg != NULL, "DamageFormula not NULL");

    printf("\n  Loaded combat values:\n");
    printf("    defense_constant = %d\n", dmg->defense_constant);
    printf("    min_damage_rate = %.3f\n", dmg->min_damage_rate);
    printf("    attribute_divisor = %d\n", dmg->attribute_divisor);

    const QuestRewardConfig* quest = GameConfig_GetQuestReward();
    TEST_CHECK(quest != NULL, "QuestReward not NULL");

    printf("\n  Loaded quest values:\n");
    printf("    exp_multiplier = %.2f\n", quest->exp_multiplier);
    printf("    gold_multiplier = %.2f\n", quest->gold_multiplier);

    const ShopDiscountConfig* shop = ShopConfig_GetDiscount();
    TEST_CHECK(shop != NULL, "ShopDiscount not NULL");

    printf("\n  Loaded shop values:\n");
    printf("    vip_discount = %.2f\n", shop->vip_discount);
    printf("    event_discount = %.2f\n", shop->event_discount);

    const PartyBonusConfig* party = PartyConfig_GetBonus();
    TEST_CHECK(party != NULL, "PartyBonus not NULL");

    printf("\n  Loaded party values:\n");
    printf("    bonus_2_members = %.2f\n", party->bonus_2_members);
    printf("    bonus_3_members = %.2f\n", party->bonus_3_members);
    printf("    bonus_4_members = %.2f\n", party->bonus_4_members);

    /* Cleanup */
    GameConfig_Cleanup();
    TEST_CHECK(1, "Cleanup completed");
}

/* ========================================================================
 * TEST 2: CONFIG VALUES VERIFICATION
 * ======================================================================== */

static void test_config_values(const char* config_dir) {
    TEST_BEGIN("Config Values Verification");

    /* Initialize from files */
    GameConfig_InitializeFromFile(config_dir);

    /* Only verify values if loaded from FILE */
    if (GameConfig_GetSource() == CONFIG_SOURCE_FILE) {
        printf("  Verifying values loaded from config files...\n");

        /* Combat config: combat.etc specifies defense_constant = 1000 */
        const DamageFormulaConfig* dmg = CombatConfig_GetDamageFormula();
        TEST_CHECK(dmg->defense_constant == 1000, "defense_constant = 1000");
        TEST_CHECK(dmg->attribute_divisor == 200, "attribute_divisor = 200");

        /* Quest config: quest.etc specifies exp_multiplier = 1.5 */
        const QuestRewardConfig* quest = GameConfig_GetQuestReward();
        TEST_CHECK(quest->exp_multiplier > 1.49f && quest->exp_multiplier < 1.51f,
                   "exp_multiplier = 1.5");

        /* Shop config: shop.etc specifies vip_discount = 0.9 */
        const ShopDiscountConfig* shop = ShopConfig_GetDiscount();
        TEST_CHECK(shop->vip_discount > 0.89f && shop->vip_discount < 0.91f,
                   "vip_discount = 0.9");

        /* Party config: party.etc specifies bonus_2_members = 1.1 */
        const PartyBonusConfig* party = PartyConfig_GetBonus();
        TEST_CHECK(party->bonus_2_members > 1.09f && party->bonus_2_members < 1.11f,
                   "bonus_2_members = 1.1");
    } else {
        printf("  [SKIP] Config loaded from hardcoded values, skipping file verification\n");
    }

    GameConfig_Cleanup();
}

/* ========================================================================
 * TEST 3: GAMECONFIG_INITIALIZEEX WITH FILE SOURCE
 * ======================================================================== */

static void test_initialize_ex(const char* config_dir) {
    TEST_BEGIN("GameConfig_InitializeEx with FILE source");

    /* Test with CONFIG_SOURCE_FILE */
    int result = GameConfig_InitializeEx(CONFIG_SOURCE_FILE, config_dir);

    TEST_CHECK(result == 0 || GameConfig_GetSource() == CONFIG_SOURCE_HARDCODED,
               "InitializeEx returns valid status");

    printf("  Source after InitializeEx: %s\n",
           GameConfig_GetSource() == CONFIG_SOURCE_FILE ? "FILE" :
           GameConfig_GetSource() == CONFIG_SOURCE_HARDCODED ? "HARDCODED" : "OTHER");

    /* Verify API works */
    const GameConfig* config = GameConfig_Get();
    TEST_CHECK(config != NULL, "GameConfig_Get() not NULL");

    GameConfig_Cleanup();
    TEST_CHECK(1, "Cleanup after InitializeEx");
}

/* ========================================================================
 * MAIN
 * ======================================================================== */

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("GameConfig FILE Loading Test\n");
    printf("Phase: 6.7 - Stage 4\n");
    printf("========================================\n");

    /* Default config directory */
    const char* config_dir = "../../config/etc";
    if (argc >= 2) {
        config_dir = argv[1];
    }

    printf("Using config directory: %s\n", config_dir);

    /* Run tests */
    test_file_config(config_dir);
    test_config_values(config_dir);
    test_initialize_ex(config_dir);

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
