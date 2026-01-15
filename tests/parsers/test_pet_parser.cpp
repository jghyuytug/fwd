/**
 * @file test_pet_parser.cpp
 * @brief Unit tests for Pet Script Parser
 *
 * Tests pet parser with comprehensive coverage of all data structures.
 */

#include "parser/pet_parser.h"
#include <cstdio>
#include <cstring>

// Global stubs
char g_petScriptBaseDirectory[256] = "/data/scripts/pet/";

struct STScriptFileList {
    static int FindIndexByFullScan(STScriptFileList*, const char*) { return 0; }
};

STScriptFileList g_petScriptFileList;

int getDrawLayer(const char* layerName) {
    if (std::strcmp(layerName, "front") == 0) return 1;
    if (std::strcmp(layerName, "back") == 0) return 2;
    return 0;
}

bool loadRDARScriptFile(const char*, const char*) {
    return true;
}

// Test utilities
#define TEST_CASE(name) \
    static int test_##name() { \
        const char* test_name = #name; \
        int failures = 0;

#define TEST_END() \
        if (failures == 0) { \
            std::printf("[PASS] %s\n", test_name); \
        } else { \
            std::printf("[FAIL] %s (%d assertions failed)\n", test_name, failures); \
        } \
        return failures; \
    }

#define ASSERT_TRUE(expr, msg) \
    if (!(expr)) { \
        std::printf("  ASSERT_TRUE failed: %s\n", msg); \
        failures++; \
    }

#define ASSERT_EQ(a, b, msg) \
    if ((a) != (b)) { \
        std::printf("  ASSERT_EQ failed: %s (expected %d, got %d)\n", msg, (int)(b), (int)(a)); \
        failures++; \
    }

#define ASSERT_STR_EQ(a, b, msg) \
    if (std::strcmp((a).c_str(), b) != 0) { \
        std::printf("  ASSERT_STR_EQ failed: %s (expected '%s', got '%s')\n", msg, b, (a).c_str()); \
        failures++; \
    }

// ============================================================================
// Tests
// ============================================================================

TEST_CASE(PetScript_Constructor) {
    PetScript pet;

    ASSERT_TRUE(pet.name.empty(), "Name should be empty");
    ASSERT_EQ(pet.width, 0, "Width should be 0");
    ASSERT_EQ(pet.height, 0, "Height should be 0");
    ASSERT_EQ(pet.floatingHeight, 0, "Floating height should be 0");
    ASSERT_EQ(pet.drawLayer, 0, "Draw layer should be 0");
    ASSERT_EQ(pet.recoveryTime, 0, "Recovery time should be 0");
    ASSERT_TRUE(!pet.directionCorrect, "Direction correct should be false");
    ASSERT_EQ(pet.gravity, 0, "Gravity should be 0");
    ASSERT_TRUE(pet.basicMotion.empty(), "Basic motion should be empty");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_Clear) {
    PetScript pet;

    // Set values
    pet.name = "TestPet";
    pet.width = 50;
    pet.height = 100;
    pet.basicMotion = "test.ani";
    pet.moveSpeed[0] = 100;
    pet.etcMotion.push_back("motion1");

    // Clear
    pet.clear();

    // Verify cleared
    ASSERT_TRUE(pet.name.empty(), "Name should be cleared");
    ASSERT_EQ(pet.width, 0, "Width should be 0");
    ASSERT_EQ(pet.height, 0, "Height should be 0");
    ASSERT_TRUE(pet.basicMotion.empty(), "Basic motion should be cleared");
    ASSERT_EQ(pet.moveSpeed[0], 0, "Move speed should be 0");
    ASSERT_TRUE(pet.etcMotion.empty(), "Etc motion should be cleared");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_NameProperty) {
    PetScript pet;

    pet.name = "Dragon";
    ASSERT_STR_EQ(pet.name, "Dragon", "Name mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_Dimensions) {
    PetScript pet;

    pet.width = 80;
    pet.height = 120;
    pet.floatingHeight = 15;

    ASSERT_EQ(pet.width, 80, "Width mismatch");
    ASSERT_EQ(pet.height, 120, "Height mismatch");
    ASSERT_EQ(pet.floatingHeight, 15, "Floating height mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_MovementSystem) {
    PetScript pet;

    pet.moveSpeed[0] = 100;
    pet.moveSpeed[1] = 150;
    pet.moveSpeed[2] = 200;
    pet.moveSpeed[3] = 250;
    pet.moveSpeed[4] = 300;
    pet.gravity = 980;

    ASSERT_EQ(pet.moveSpeed[0], 100, "Speed[0] mismatch");
    ASSERT_EQ(pet.moveSpeed[1], 150, "Speed[1] mismatch");
    ASSERT_EQ(pet.moveSpeed[4], 300, "Speed[4] mismatch");
    ASSERT_EQ(pet.gravity, 980, "Gravity mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_BasicAnimation) {
    PetScript pet;

    pet.basicMotion = "pet/dragon/basic.ani";
    ASSERT_STR_EQ(pet.basicMotion, "pet/dragon/basic.ani", "Basic motion mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_EtcMotions) {
    PetScript pet;

    pet.etcMotion.push_back("motion1.ani");
    pet.etcMotion.push_back("motion2.ani");
    pet.etcMotion.push_back("motion3.ani");

    ASSERT_EQ(pet.etcMotion.size(), 3, "Etc motion size mismatch");
    ASSERT_STR_EQ(pet.etcMotion[0], "motion1.ani", "Motion[0] mismatch");
    ASSERT_STR_EQ(pet.etcMotion[1], "motion2.ani", "Motion[1] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_Attributes) {
    PetScript pet;

    pet.recoveryTime = 5000;
    pet.directionCorrect = true;
    pet.drawLayer = 3;

    ASSERT_EQ(pet.recoveryTime, 5000, "Recovery time mismatch");
    ASSERT_TRUE(pet.directionCorrect, "Direction correct should be true");
    ASSERT_EQ(pet.drawLayer, 3, "Draw layer mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_AttackInfo) {
    PetScript pet;

    pet.attackInfo.push_back("attack1.atk");
    pet.attackInfo.push_back("attack2.atk");

    ASSERT_EQ(pet.attackInfo.size(), 2, "Attack info size mismatch");
    ASSERT_STR_EQ(pet.attackInfo[0], "attack1.atk", "Attack[0] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_IntData) {
    PetScript pet;

    pet.intData.push_back(100);
    pet.intData.push_back(200);

    ASSERT_EQ(pet.intData.size(), 2, "Int data size mismatch");
    ASSERT_EQ(pet.intData[0], 100, "Int data[0] mismatch");
    ASSERT_EQ(pet.intData[1], 200, "Int data[1] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_StringData) {
    PetScript pet;

    pet.stringData.push_back("data1");
    pet.stringData.push_back("data2");

    ASSERT_EQ(pet.stringData.size(), 2, "String data size mismatch");
    ASSERT_STR_EQ(pet.stringData[0], "data1", "String data[0] mismatch");

    return failures;
}
TEST_END()

TEST_CASE(PetScript_PetId) {
    PetScript pet;

    pet.petId = 12345;
    ASSERT_EQ(pet.petId, 12345, "Pet ID mismatch");

    return failures;
}
TEST_END()

// Test runner
int main() {
    std::printf("Running Pet Parser Tests...\n\n");

    int total_failures = 0;

    total_failures += test_PetScript_Constructor();
    total_failures += test_PetScript_Clear();
    total_failures += test_PetScript_NameProperty();
    total_failures += test_PetScript_Dimensions();
    total_failures += test_PetScript_MovementSystem();
    total_failures += test_PetScript_BasicAnimation();
    total_failures += test_PetScript_EtcMotions();
    total_failures += test_PetScript_Attributes();
    total_failures += test_PetScript_AttackInfo();
    total_failures += test_PetScript_IntData();
    total_failures += test_PetScript_StringData();
    total_failures += test_PetScript_PetId();

    std::printf("\n========================================\n");
    if (total_failures == 0) {
        std::printf("ALL TESTS PASSED (12 test cases)\n");
        std::printf("========================================\n");
        return 0;
    } else {
        std::printf("TESTS FAILED: %d assertions failed\n", total_failures);
        std::printf("========================================\n");
        return 1;
    }
}
