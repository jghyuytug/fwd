/**
 * @file test_table_parser.cpp
 * @brief Test suite for Arad_Script table parser
 *
 * Comprehensive tests for table loading, parsing, and data access.
 */

#include "parser/table_parser.h"
#include <iostream>
#include <cstring>
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

#define ASSERT_STR_EQ(a, b) \
    do { \
        if (std::strcmp((a), (b)) != 0) { \
            throw "Assertion failed: strcmp(" #a ", " #b ") == 0"; \
        } \
    } while (0)

#define ASSERT_FLOAT_EQ(a, b) \
    do { \
        float diff = (a) - (b); \
        if (diff < 0) diff = -diff; \
        if (diff > 0.0001f) { \
            throw "Assertion failed: " #a " ~= " #b; \
        } \
    } while (0)

// ===== Test Data =====

// Simple 3x3 table
const char* TEST_TABLE_SIMPLE =
    "[Skills]\n"
    "name    damage  cooldown\n"
    "fireball    100    5.0\n"
    "icebolt    80    3.5\n"
    "lightning    120    7.0\n";

// Table with column aliases
const char* TEST_TABLE_ALIASES =
    "[Items]\n"
    "name    dmg/damage    def/defense\n"
    "sword    50    10\n"
    "shield    0    30\n";

// Table with many columns (to test expansion)
const char* TEST_TABLE_LARGE =
    "[Stats]\n"
    "name c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15 c16 c17 c18 c19 c20 c21 c22 c23 c24 c25 c26 c27 c28 c29 c30 c31 c32 c33 c34 c35\n"
    "player 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35\n";

// Empty section
const char* TEST_TABLE_EMPTY =
    "[Empty]\n"
    "col1    col2    col3\n";

// Multiple sections
const char* TEST_TABLE_MULTI_SECTION =
    "[First]\n"
    "name    value\n"
    "a    100\n"
    "\n"
    "[Second]\n"
    "name    value\n"
    "b    200\n";

// ===== Basic Tests =====

TEST(Constructor_Default) {
    Arad_Script table;
    ASSERT_EQ(table.GetColCount(), 0);
    ASSERT_EQ(table.GetRowCount(), 0);
}

TEST(Constructor_WithPath) {
    Arad_Script table("test.txt");
    ASSERT_EQ(table.GetColCount(), 0);
    ASSERT_EQ(table.GetRowCount(), 0);
}

TEST(Create_Allocates_Storage) {
    Arad_Script table;
    ASSERT(table.Create());
    ASSERT_EQ(table.GetColCount(), 0);
    ASSERT_EQ(table.GetRowCount(), 0);
}

// ===== Load Tests =====

TEST(Load_Simple_Table) {
    Arad_Script table;
    ASSERT(table.Load(TEST_TABLE_SIMPLE, "Skills"));
    ASSERT_EQ(table.GetColCount(), 3);
    ASSERT_EQ(table.GetRowCount(), 3);
}

TEST(Load_NonExistent_Section) {
    Arad_Script table;
    ASSERT(!table.Load(TEST_TABLE_SIMPLE, "NonExistent"));
}

TEST(Load_Multiple_Sections_First) {
    Arad_Script table;
    ASSERT(table.Load(TEST_TABLE_MULTI_SECTION, "First"));
    ASSERT_EQ(table.GetRowCount(), 1);
    ASSERT_STR_EQ(table.GetString(0, 1), "100");
}

TEST(Load_Multiple_Sections_Second) {
    Arad_Script table;
    ASSERT(table.Load(TEST_TABLE_MULTI_SECTION, "Second"));
    ASSERT_EQ(table.GetRowCount(), 1);
    ASSERT_STR_EQ(table.GetString(0, 1), "200");
}

TEST(Load_Empty_Table) {
    Arad_Script table;
    ASSERT(table.Load(TEST_TABLE_EMPTY, "Empty"));
    ASSERT_EQ(table.GetColCount(), 3);
    ASSERT_EQ(table.GetRowCount(), 0);
}

TEST(Load_Large_Table_Expands_Columns) {
    Arad_Script table;
    ASSERT(table.Load(TEST_TABLE_LARGE, "Stats"));
    ASSERT_EQ(table.GetColCount(), 36);  // name + 35 columns
    ASSERT_EQ(table.GetRowCount(), 1);
}

// ===== Name Lookup Tests =====

TEST(GetIdx4Col_Valid_Name) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetIdx4Col("name"), 0);
    ASSERT_EQ(table.GetIdx4Col("damage"), 1);
    ASSERT_EQ(table.GetIdx4Col("cooldown"), 2);
}

TEST(GetIdx4Col_Invalid_Name) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetIdx4Col("invalid"), -1);
}

TEST(GetIdx4Col_Alias_Support) {
    Arad_Script table;
    table.Load(TEST_TABLE_ALIASES, "Items");
    ASSERT_EQ(table.GetIdx4Col("dmg"), 1);
    ASSERT_EQ(table.GetIdx4Col("damage"), 1);
    ASSERT_EQ(table.GetIdx4Col("def"), 2);
    ASSERT_EQ(table.GetIdx4Col("defense"), 2);
}

TEST(GetIdx4Row_Valid_Name) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetIdx4Row("fireball"), 0);
    ASSERT_EQ(table.GetIdx4Row("icebolt"), 1);
    ASSERT_EQ(table.GetIdx4Row("lightning"), 2);
}

TEST(GetIdx4Row_Invalid_Name) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetIdx4Row("invalid"), -1);
}

// ===== Reverse Lookup Tests =====

TEST(GetColName_Valid_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    // Note: With aliases, only one name is returned (first one registered)
    ASSERT_STR_EQ(table.GetColName(0).c_str(), "name");
    ASSERT_STR_EQ(table.GetColName(1).c_str(), "damage");
    ASSERT_STR_EQ(table.GetColName(2).c_str(), "cooldown");
}

TEST(GetRowName_Valid_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetRowName(0).c_str(), "fireball");
    ASSERT_STR_EQ(table.GetRowName(1).c_str(), "icebolt");
    ASSERT_STR_EQ(table.GetRowName(2).c_str(), "lightning");
}

// ===== GetString Tests =====

TEST(GetString_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetString(0, 0), "fireball");
    ASSERT_STR_EQ(table.GetString(0, 1), "100");
    ASSERT_STR_EQ(table.GetString(0, 2), "5.0");
}

TEST(GetString_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetString("fireball", "damage"), "100");
    ASSERT_STR_EQ(table.GetString("icebolt", "cooldown"), "3.5");
    ASSERT_STR_EQ(table.GetString("lightning", "name"), "lightning");
}

TEST(GetString_By_RowName_ColIndex) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetString("fireball", 1), "100");
    ASSERT_STR_EQ(table.GetString("icebolt", 2), "3.5");
}

TEST(GetString_By_RowIndex_ColName) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetString(0, "damage"), "100");
    ASSERT_STR_EQ(table.GetString(1, "cooldown"), "3.5");
}

TEST(GetString_Invalid_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_STR_EQ(table.GetString(10, 10), "");
    ASSERT_STR_EQ(table.GetString(-1, 0), "");
}

// ===== GetInt Tests =====

TEST(GetInt_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetInt(0, 1), 100);
    ASSERT_EQ(table.GetInt(1, 1), 80);
    ASSERT_EQ(table.GetInt(2, 1), 120);
}

TEST(GetInt_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetInt("fireball", "damage"), 100);
    ASSERT_EQ(table.GetInt("icebolt", "damage"), 80);
    ASSERT_EQ(table.GetInt("lightning", "damage"), 120);
}

TEST(GetInt_By_RowName_ColIndex) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetInt("fireball", 1), 100);
}

TEST(GetInt_By_RowIndex_ColName) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_EQ(table.GetInt(0, "damage"), 100);
}

// ===== GetFloat Tests =====

TEST(GetFloat_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_FLOAT_EQ(table.GetFloat(0, 2), 5.0f);
    ASSERT_FLOAT_EQ(table.GetFloat(1, 2), 3.5f);
    ASSERT_FLOAT_EQ(table.GetFloat(2, 2), 7.0f);
}

TEST(GetFloat_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_FLOAT_EQ(table.GetFloat("fireball", "cooldown"), 5.0f);
    ASSERT_FLOAT_EQ(table.GetFloat("icebolt", "cooldown"), 3.5f);
}

TEST(GetFloat_By_RowName_ColIndex) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_FLOAT_EQ(table.GetFloat("fireball", 2), 5.0f);
}

TEST(GetFloat_By_RowIndex_ColName) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    ASSERT_FLOAT_EQ(table.GetFloat(0, "cooldown"), 5.0f);
}

// ===== GetDouble Tests =====

TEST(GetDouble_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    double val = table.GetDouble(0, 2);
    ASSERT(val > 4.99 && val < 5.01);
}

TEST(GetDouble_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    double val = table.GetDouble("fireball", "cooldown");
    ASSERT(val > 4.99 && val < 5.01);
}

// ===== SetString Tests =====

TEST(SetString_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetString(0, 0, "meteor");
    ASSERT_STR_EQ(table.GetString(0, 0), "meteor");
}

TEST(SetString_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetString("fireball", "name", "meteor");
    ASSERT_STR_EQ(table.GetString("fireball", "name"), "meteor");
}

TEST(SetString_By_RowName_ColIndex) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetString("fireball", 0, "meteor");
    ASSERT_STR_EQ(table.GetString("fireball", 0), "meteor");
}

TEST(SetString_By_RowIndex_ColName) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetString(0, "name", "meteor");
    ASSERT_STR_EQ(table.GetString(0, "name"), "meteor");
}

TEST(SetString_Truncates_Long_Values) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");

    // Create a string longer than 128 bytes
    char longStr[200];
    for (int i = 0; i < 199; i++) {
        longStr[i] = 'A';
    }
    longStr[199] = '\0';

    table.SetString(0, 0, longStr);
    const char* result = table.GetString(0, 0);

    // Should be truncated to 127 bytes (128 - 1 for null terminator)
    ASSERT_EQ(std::strlen(result), 127);
}

// ===== SetInt Tests =====

TEST(SetInt_By_Index) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetInt(0, 1, 999);
    ASSERT_EQ(table.GetInt(0, 1), 999);
}

TEST(SetInt_By_Names) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetInt("fireball", "damage", 999);
    ASSERT_EQ(table.GetInt("fireball", "damage"), 999);
}

TEST(SetInt_By_RowName_ColIndex) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetInt("fireball", 1, 999);
    ASSERT_EQ(table.GetInt("fireball", 1), 999);
}

TEST(SetInt_By_RowIndex_ColName) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetInt(0, "damage", 999);
    ASSERT_EQ(table.GetInt(0, "damage"), 999);
}

// ===== Edge Case Tests =====

TEST(Edge_Invalid_SetString_Does_Not_Crash) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetString(100, 100, "test");  // Should not crash
    table.SetString(-1, 0, "test");     // Should not crash
}

TEST(Edge_Invalid_SetInt_Does_Not_Crash) {
    Arad_Script table;
    table.Load(TEST_TABLE_SIMPLE, "Skills");
    table.SetInt(100, 100, 42);  // Should not crash
    table.SetInt(-1, 0, 42);     // Should not crash
}

TEST(Edge_Empty_Cell_GetInt_Returns_Zero) {
    Arad_Script table;
    table.Load(TEST_TABLE_EMPTY, "Empty");
    // No rows, but columns exist
    ASSERT_EQ(table.GetInt(0, 0), 0);
}

// ===== Main Test Runner =====

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Arad_Script Table Parser Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Basic tests
    run_test_Constructor_Default();
    run_test_Constructor_WithPath();
    run_test_Create_Allocates_Storage();

    // Load tests
    run_test_Load_Simple_Table();
    run_test_Load_NonExistent_Section();
    run_test_Load_Multiple_Sections_First();
    run_test_Load_Multiple_Sections_Second();
    run_test_Load_Empty_Table();
    run_test_Load_Large_Table_Expands_Columns();

    // Name lookup tests
    run_test_GetIdx4Col_Valid_Name();
    run_test_GetIdx4Col_Invalid_Name();
    run_test_GetIdx4Col_Alias_Support();
    run_test_GetIdx4Row_Valid_Name();
    run_test_GetIdx4Row_Invalid_Name();

    // Reverse lookup tests
    run_test_GetColName_Valid_Index();
    run_test_GetRowName_Valid_Index();

    // GetString tests
    run_test_GetString_By_Index();
    run_test_GetString_By_Names();
    run_test_GetString_By_RowName_ColIndex();
    run_test_GetString_By_RowIndex_ColName();
    run_test_GetString_Invalid_Index();

    // GetInt tests
    run_test_GetInt_By_Index();
    run_test_GetInt_By_Names();
    run_test_GetInt_By_RowName_ColIndex();
    run_test_GetInt_By_RowIndex_ColName();

    // GetFloat tests
    run_test_GetFloat_By_Index();
    run_test_GetFloat_By_Names();
    run_test_GetFloat_By_RowName_ColIndex();
    run_test_GetFloat_By_RowIndex_ColName();

    // GetDouble tests
    run_test_GetDouble_By_Index();
    run_test_GetDouble_By_Names();

    // SetString tests
    run_test_SetString_By_Index();
    run_test_SetString_By_Names();
    run_test_SetString_By_RowName_ColIndex();
    run_test_SetString_By_RowIndex_ColName();
    run_test_SetString_Truncates_Long_Values();

    // SetInt tests
    run_test_SetInt_By_Index();
    run_test_SetInt_By_Names();
    run_test_SetInt_By_RowName_ColIndex();
    run_test_SetInt_By_RowIndex_ColName();

    // Edge case tests
    run_test_Edge_Invalid_SetString_Does_Not_Crash();
    run_test_Edge_Invalid_SetInt_Does_Not_Crash();
    run_test_Edge_Empty_Cell_GetInt_Returns_Zero();

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
