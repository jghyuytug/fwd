/**
 * @file test_itemshop_parser.cpp
 * @brief Unit tests for Item Shop Script Parser
 *
 * Tests all 10 section types:
 * - [NPC], [message], [type], [only buy]
 * - [sell item], [sell limit item], [tab name]
 * - [one a day start time], [one a day item]
 *
 * @date 2025-01-XX
 */

#include "parser/itemshop_parser.h"
#include "parser/scan_utils.h"
#include "lexer/rdar_loader.h"
#include <cassert>
#include <cstring>
#include <iostream>

// ============================================================================
// Mock Implementations
// ============================================================================

char g_itemShopScriptBaseDirectory[256] = "/data/scripts/itemshop/";
STScriptFileList g_itemShopScriptFileList;

static const char* g_mockScriptData = nullptr;

bool loadRDARScriptFile(const char* baseDir, const char* filename) {
    if (g_mockScriptData == nullptr) {
        return false;
    }

    // Initialize lexer with mock data
    DNFLexWrapper::setInputStream(&g_dnfLex, g_mockScriptData, std::strlen(g_mockScriptData));
    return true;
}

int STScriptFileList::FindIndexByFullScan(STScriptFileList* list, const char* filename) {
    // Mock implementation: return fixed index
    (void)list;
    (void)filename;
    return 42;  // Arbitrary script index
}

// ============================================================================
// Test Utilities
// ============================================================================

static void setMockScriptData(const char* data) {
    g_mockScriptData = data;
}

static void clearMockScriptData() {
    g_mockScriptData = nullptr;
}

// ============================================================================
// Test Cases
// ============================================================================

/**
 * @test Test [NPC] section parsing
 */
void test_parse_npc_section() {
    const char* scriptData =
        "[NPC]\n"
        "100234\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_npc.shop");

    assert(result == 1);
    assert(shop.npcId == 100234);
    assert(shop.scriptIndex == 42);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_npc_section\n";
}

/**
 * @test Test [message] section parsing
 */
void test_parse_message_section() {
    const char* scriptData =
        "[message]\n"
        "`Welcome to my shop!`\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_message.shop");

    assert(result == 1);
    assert(shop.message == "Welcome to my shop!");

    clearMockScriptData();
    std::cout << "[PASS] test_parse_message_section\n";
}

/**
 * @test Test [type] section with [etc shop]
 */
void test_parse_type_etc_shop() {
    const char* scriptData =
        "[type]\n"
        "[etc shop]\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_type_etc.shop");

    assert(result == 1);
    assert(shop.shopType == ShopType::ETC_SHOP);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_type_etc_shop\n";
}

/**
 * @test Test [type] section with [weapon shop]
 */
void test_parse_type_weapon_shop() {
    const char* scriptData =
        "[type]\n"
        "[weapon shop]\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_type_weapon.shop");

    assert(result == 1);
    assert(shop.shopType == ShopType::WEAPON_SHOP);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_type_weapon_shop\n";
}

/**
 * @test Test [type] section with [disjoint shop]
 */
void test_parse_type_disjoint_shop() {
    const char* scriptData =
        "[type]\n"
        "[disjoint shop]\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_type_disjoint.shop");

    assert(result == 1);
    assert(shop.shopType == ShopType::DISJOINT_SHOP);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_type_disjoint_shop\n";
}

/**
 * @test Test [only buy] section with flag = 1
 */
void test_parse_only_buy_true() {
    const char* scriptData =
        "[only buy]\n"
        "1\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_only_buy.shop");

    assert(result == 1);
    assert(shop.onlyBuy == true);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_only_buy_true\n";
}

/**
 * @test Test [only buy] section with flag = 0
 */
void test_parse_only_buy_false() {
    const char* scriptData =
        "[only buy]\n"
        "0\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_only_buy_false.shop");

    assert(result == 1);
    assert(shop.onlyBuy == false);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_only_buy_false\n";
}

/**
 * @test Test [sell item] section with multiple items
 */
void test_parse_sell_item_list() {
    const char* scriptData =
        "[sell item]\n"
        "10001\n"
        "10002\n"
        "10003\n"
        "10004\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_sell_items.shop");

    assert(result == 1);
    assert(shop.sellItems.size() == 4);
    assert(shop.sellItems[0] == 10001);
    assert(shop.sellItems[1] == 10002);
    assert(shop.sellItems[2] == 10003);
    assert(shop.sellItems[3] == 10004);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_sell_item_list\n";
}

/**
 * @test Test [sell limit item] section
 */
void test_parse_sell_limit_item() {
    const char* scriptData =
        "[sell limit item]\n"
        "20001\n"
        "20002\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_limit_items.shop");

    assert(result == 1);
    assert(shop.sellLimitItems.size() == 2);
    assert(shop.sellLimitItems[0] == 20001);
    assert(shop.sellLimitItems[1] == 20002);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_sell_limit_item\n";
}

/**
 * @test Test [tab name] section with multiple tabs
 */
void test_parse_tab_names() {
    const char* scriptData =
        "[tab name]\n"
        "`Weapons`\n"
        "`Armor`\n"
        "`Consumables`\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_tabs.shop");

    assert(result == 1);
    assert(shop.tabNames.size() == 3);
    assert(shop.tabNames[0] == "Weapons");
    assert(shop.tabNames[1] == "Armor");
    assert(shop.tabNames[2] == "Consumables");

    clearMockScriptData();
    std::cout << "[PASS] test_parse_tab_names\n";
}

/**
 * @test Test [one a day start time] section
 */
void test_parse_one_a_day_start_time() {
    const char* scriptData =
        "[one a day start time]\n"
        "2025 1 15 0\n";  // January 15, 2025 at midnight

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_daily_time.shop");

    assert(result == 1);
    assert(shop.oneADayStartTime > 0);

    // Verify timestamp (should be around January 15, 2025)
    struct tm* timeInfo = std::localtime(&shop.oneADayStartTime);
    assert(timeInfo->tm_year == 125);  // 2025 - 1900
    assert(timeInfo->tm_mon == 0);     // January (0-11)
    assert(timeInfo->tm_mday == 15);   // 15th day
    assert(timeInfo->tm_hour == 0);    // Midnight

    clearMockScriptData();
    std::cout << "[PASS] test_parse_one_a_day_start_time\n";
}

/**
 * @test Test [one a day item] section with single group
 */
void test_parse_one_a_day_item_single_group() {
    const char* scriptData =
        "[one a day item]\n"
        "30001\n"
        "30002\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_daily_single.shop");

    assert(result == 1);
    assert(shop.oneADayItems.size() == 1);
    assert(shop.oneADayItems[0].size() == 2);
    assert(shop.oneADayItems[0][0] == 30001);
    assert(shop.oneADayItems[0][1] == 30002);
    assert(shop.hasOneADayItems == true);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_one_a_day_item_single_group\n";
}

/**
 * @test Test [one a day item] section with multiple groups
 */
void test_parse_one_a_day_item_multiple_groups() {
    const char* scriptData =
        "[one a day item]\n"
        "30001\n"
        "30002\n"
        "[one a day item]\n"
        "30003\n"
        "[one a day item]\n"
        "30004\n"
        "30005\n"
        "30006\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "test_daily_multiple.shop");

    assert(result == 1);
    assert(shop.oneADayItems.size() == 3);

    // Group 1: 2 items
    assert(shop.oneADayItems[0].size() == 2);
    assert(shop.oneADayItems[0][0] == 30001);
    assert(shop.oneADayItems[0][1] == 30002);

    // Group 2: 1 item
    assert(shop.oneADayItems[1].size() == 1);
    assert(shop.oneADayItems[1][0] == 30003);

    // Group 3: 3 items
    assert(shop.oneADayItems[2].size() == 3);
    assert(shop.oneADayItems[2][0] == 30004);
    assert(shop.oneADayItems[2][1] == 30005);
    assert(shop.oneADayItems[2][2] == 30006);

    assert(shop.hasOneADayItems == true);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_one_a_day_item_multiple_groups\n";
}

/**
 * @test Test complete shop script with all sections
 */
void test_parse_complete_shop_script() {
    const char* scriptData =
        "[NPC]\n"
        "100234\n"
        "\n"
        "[type]\n"
        "[weapon shop]\n"
        "\n"
        "[message]\n"
        "`Welcome to the weapon shop!`\n"
        "\n"
        "[only buy]\n"
        "0\n"
        "\n"
        "[sell item]\n"
        "10001\n"
        "10002\n"
        "10003\n"
        "\n"
        "[sell limit item]\n"
        "20001\n"
        "\n"
        "[tab name]\n"
        "`Swords`\n"
        "`Axes`\n"
        "\n"
        "[one a day start time]\n"
        "2025 1 20 6\n"
        "\n"
        "[one a day item]\n"
        "30001\n"
        "30002\n";

    setMockScriptData(scriptData);

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "complete_shop.shop");

    assert(result == 1);
    assert(shop.scriptIndex == 42);
    assert(shop.npcId == 100234);
    assert(shop.shopType == ShopType::WEAPON_SHOP);
    assert(shop.message == "Welcome to the weapon shop!");
    assert(shop.onlyBuy == false);
    assert(shop.sellItems.size() == 3);
    assert(shop.sellLimitItems.size() == 1);
    assert(shop.tabNames.size() == 2);
    assert(shop.oneADayStartTime > 0);
    assert(shop.oneADayItems.size() == 1);
    assert(shop.hasOneADayItems == true);

    clearMockScriptData();
    std::cout << "[PASS] test_parse_complete_shop_script\n";
}

/**
 * @test Test file not found error
 */
void test_file_not_found() {
    clearMockScriptData();  // No mock data = file not found

    ItemShopScript shop;
    int result = importItemShopScript(&shop, "nonexistent.shop");

    assert(result == 0);  // Should fail

    std::cout << "[PASS] test_file_not_found\n";
}

/**
 * @test Test clear() method
 */
void test_clear_method() {
    ItemShopScript shop;
    shop.scriptIndex = 123;
    shop.shopType = ShopType::WEAPON_SHOP;
    shop.npcId = 456;
    shop.onlyBuy = true;
    shop.message = "Test message";
    shop.sellItems.push_back(1);
    shop.tabNames.push_back("Tab");
    shop.sellLimitItems.push_back(2);
    shop.oneADayItems.push_back(std::vector<int>{3, 4});
    shop.hasOneADayItems = true;
    shop.oneADayStartTime = 12345;

    shop.clear();

    assert(shop.scriptIndex == 0);
    assert(shop.shopType == ShopType::ETC_SHOP);
    assert(shop.npcId == 0);
    assert(shop.onlyBuy == false);
    assert(shop.message.empty());
    assert(shop.sellItems.empty());
    assert(shop.tabNames.empty());
    assert(shop.sellLimitItems.empty());
    assert(shop.oneADayItems.empty());
    assert(shop.hasOneADayItems == false);
    assert(shop.oneADayStartTime == 0);

    std::cout << "[PASS] test_clear_method\n";
}

// ============================================================================
// Test Runner
// ============================================================================

int main() {
    std::cout << "=== ItemShop Parser Unit Tests ===\n\n";

    test_parse_npc_section();
    test_parse_message_section();
    test_parse_type_etc_shop();
    test_parse_type_weapon_shop();
    test_parse_type_disjoint_shop();
    test_parse_only_buy_true();
    test_parse_only_buy_false();
    test_parse_sell_item_list();
    test_parse_sell_limit_item();
    test_parse_tab_names();
    test_parse_one_a_day_start_time();
    test_parse_one_a_day_item_single_group();
    test_parse_one_a_day_item_multiple_groups();
    test_parse_complete_shop_script();
    test_file_not_found();
    test_clear_method();

    std::cout << "\n=== All 16 tests passed! ===\n";
    return 0;
}
