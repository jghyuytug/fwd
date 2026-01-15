/**
 * @file test_stagemap_parser.cpp
 * @brief Test suite for stage map parser
 */

#include "parser/stagemap_parser.h"
#include <iostream>
#include <cassert>

// ===== Test Utilities =====

static int g_testCount = 0;
static int g_passCount = 0;
static int g_failCount = 0;

#define TEST(name) \
    static void test_##name(); \
    static void run_test_##name() { \
        g_testCount++; \
        std::cout << "Running test: " << #name << " ... "; \
        try { \
            test_##name(); \
            g_passCount++; \
            std::cout << "PASS" << std::endl; \
        } catch (const char* msg) { \
            g_failCount++; \
            std::cout << "FAIL: " << msg << std::endl; \
        } \
    } \
    static void test_##name()

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw "Assertion failed: " #condition; \
        } \
    } while (0)

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            throw "Assertion failed: " #a " == " #b; \
        } \
    } while (0)

// ===== Mock Functions =====

bool loadRDARScriptFile(const char* dir, const char* filename) {
    (void)dir;
    (void)filename;
    return true;  // Mock success
}

struct STScriptFileList {};
STScriptFileList g_stageMapScriptFileList;
const char g_stageMapScriptBaseDirectory[] = "stagemap";

// ===== Tests =====

TEST(StageMapScript_Constructor) {
    stagemap::StageMapScript map;
    ASSERT_EQ(map.mapId, 0);
    ASSERT_EQ(map.dungeonId, 0);
    ASSERT_EQ(map.mapType, 0);
    ASSERT(map.name.empty());
    ASSERT(map.image.empty());
    ASSERT_EQ(map.areas.size(), 0);
}

TEST(StageMapScript_Clear) {
    stagemap::StageMapScript map;
    map.mapId = 123;
    map.dungeonId = 456;
    map.name = "Test Map";
    map.image = "test.img";

    stagemap::AreaData area;
    area.areaId = 1;
    map.areas.push_back(area);

    map.clear();

    ASSERT_EQ(map.mapId, 0);
    ASSERT_EQ(map.dungeonId, 0);
    ASSERT(map.name.empty());
    ASSERT(map.image.empty());
    ASSERT_EQ(map.areas.size(), 0);
}

TEST(SlotType_Constructor) {
    stagemap::SlotType slot;
    ASSERT_EQ(slot.slotId, 0);
    ASSERT_EQ(slot.posX, 0);
    ASSERT_EQ(slot.posY, 0);
    ASSERT_EQ(slot.width, 0);
    ASSERT_EQ(slot.height, 0);
}

TEST(SlotType_Clear) {
    stagemap::SlotType slot;
    slot.slotId = 1;
    slot.posX = 100;
    slot.posY = 200;
    slot.width = 50;
    slot.height = 50;

    slot.clear();

    ASSERT_EQ(slot.slotId, 0);
    ASSERT_EQ(slot.posX, 0);
    ASSERT_EQ(slot.posY, 0);
    ASSERT_EQ(slot.width, 0);
    ASSERT_EQ(slot.height, 0);
}

TEST(StageData_Constructor) {
    stagemap::StageData stage;
    ASSERT_EQ(stage.stageId, 0);
    ASSERT_EQ(stage.difficulty, 0);
    ASSERT_EQ(stage.minLevel, 0);
    ASSERT_EQ(stage.maxLevel, 0);
}

TEST(DistrictData_Constructor) {
    stagemap::DistrictData district;
    ASSERT_EQ(district.districtId, 0);
    ASSERT(district.slotImage.empty());
    ASSERT(district.appendImage.empty());
    ASSERT_EQ(district.appendImageOffset, 0);
    ASSERT_EQ(district.slotTypes.size(), 0);
    ASSERT_EQ(district.stages.size(), 0);
    ASSERT_EQ(district.survivalFlag, 0);
}

TEST(DistrictData_Clear) {
    stagemap::DistrictData district;
    district.districtId = 10;
    district.slotImage = "slot.img";
    district.appendImage = "append.img";
    district.appendImageOffset = 5;
    district.survivalFlag = 1;

    stagemap::SlotType slot;
    slot.slotId = 1;
    district.slotTypes[1] = slot;

    stagemap::StageData stage;
    stage.stageId = 101;
    district.stages.push_back(stage);

    district.clear();

    ASSERT_EQ(district.districtId, 0);
    ASSERT(district.slotImage.empty());
    ASSERT(district.appendImage.empty());
    ASSERT_EQ(district.appendImageOffset, 0);
    ASSERT_EQ(district.slotTypes.size(), 0);
    ASSERT_EQ(district.stages.size(), 0);
    ASSERT_EQ(district.survivalFlag, 0);
}

TEST(AreaData_Constructor) {
    stagemap::AreaData area;
    ASSERT_EQ(area.areaId, 0);
    ASSERT_EQ(area.districts.size(), 0);
}

TEST(AreaData_Clear) {
    stagemap::AreaData area;
    area.areaId = 1;

    stagemap::DistrictData district;
    district.districtId = 10;
    area.districts.push_back(district);

    area.clear();

    ASSERT_EQ(area.areaId, 0);
    ASSERT_EQ(area.districts.size(), 0);
}

// ===== Main Test Runner =====

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "StageMap Parser Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Data structure tests
    run_test_StageMapScript_Constructor();
    run_test_StageMapScript_Clear();
    run_test_SlotType_Constructor();
    run_test_SlotType_Clear();
    run_test_StageData_Constructor();
    run_test_DistrictData_Constructor();
    run_test_DistrictData_Clear();
    run_test_AreaData_Constructor();
    run_test_AreaData_Clear();

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test Results" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total:  " << g_testCount << std::endl;
    std::cout << "Passed: " << g_passCount << std::endl;
    std::cout << "Failed: " << g_failCount << std::endl;

    if (g_failCount == 0) {
        std::cout << std::endl << "All tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "Some tests FAILED!" << std::endl;
        return 1;
    }
}
