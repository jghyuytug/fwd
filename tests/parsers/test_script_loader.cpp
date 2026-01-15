/**
 * @file test_script_loader.cpp
 * @brief Test suite for Arad_ScriptLoader and script loading utilities
 *
 * Tests:
 * - Arad_ScriptLoader base class functionality
 * - ExceptionItem derived class example
 * - File loading functions
 * - List file parsing
 */

#include "parser/script_loader.h"
#include "parser/table_parser.h"
#include <iostream>
#include <cstring>
#include <cassert>
#include <map>

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

// ===== Mock DNFLex for Testing =====

// Mock lexer implementation for testing
struct DNFLex {
    void* inputStream;
    int streamSize;
    const char* mockData;
};

DNFLex g_dnfLex;

namespace DNFLexWrapper {
    bool switchInputStream(DNFLex* lex, const char* dir, const char* filename, int flags) {
        (void)dir;
        (void)flags;

        // Mock: Just verify filename is valid
        if (!filename || std::strlen(filename) == 0) {
            return false;
        }

        // Mock success
        return true;
    }

    int getInputStreamSize(DNFLex* lex) {
        // Return mock size
        if (lex->mockData) {
            return std::strlen(lex->mockData);
        }
        return 0;
    }

    void* getInputStream(DNFLex* lex) {
        // Mock: Return nullptr (not used in test)
        return nullptr;
    }
}

// Override read for mock
namespace std {
    // Normally we'd mock std::istream, but for simplicity we'll just
    // handle it in the test setup
}

// ===== Example Derived Class: ExceptionItem =====

/**
 * @class ExceptionItem
 * @brief Example script loader for material exception items
 *
 * Parses a table with format:
 * ```
 * [material_exception_item]
 * material_exception_item_index
 * 12345
 * 67890
 * ```
 */
class ExceptionItem : public Arad_ScriptLoader {
public:
    ExceptionItem() {}

    ~ExceptionItem() override {}

    /**
     * @brief Check if an item index exists
     * @param index Item index to check
     * @return true if exists, false otherwise
     */
    bool find_index(int index) const {
        return items_.find(index) != items_.end();
    }

    /**
     * @brief Get number of loaded items
     * @return Item count
     */
    size_t GetCount() const {
        return items_.size();
    }

protected:
    /**
     * @brief Parse exception item table
     * @param data File data
     * @return Error code (0 = success)
     *
     * Error codes:
     * - 0: Success
     * - 39: Load failed
     * - 42: Empty table
     * - 51: Duplicate item
     */
    int parsing(const char* data) override {
        // Create Arad_Script table parser
        Arad_Script table;

        // Load table with section name
        if (!table.Load(data, "material_exception_item")) {
            return 39;  // Load error
        }

        // Check if table has rows
        if (table.GetRowCount() == 0) {
            return 42;  // Empty table
        }

        // Parse each row
        for (int i = 0; i < table.GetRowCount(); ++i) {
            int itemIndex = table.GetInt(i, "material_exception_item_index");

            // Check for duplicate
            if (items_.find(itemIndex) != items_.end()) {
                return 51;  // Duplicate item
            }

            // Add to map
            items_[itemIndex] = itemIndex;
        }

        return 0;  // Success
    }

private:
    std::map<int, int> items_;  ///< Item index map
};

// ===== Example Derived Class: PurchaseLimitItem =====

/**
 * @class PurchaseLimitItem
 * @brief Example script loader for purchase limit items
 */
class PurchaseLimitItem : public Arad_ScriptLoader {
public:
    struct Info {
        int limit;
        int period;

        Info() : limit(0), period(0) {}
    };

    PurchaseLimitItem() {}

    ~PurchaseLimitItem() override {}

    bool find(int itemIndex, Info* outInfo) const {
        auto it = items_.find(itemIndex);
        if (it != items_.end()) {
            *outInfo = it->second;
            return true;
        }
        return false;
    }

protected:
    int parsing(const char* data) override {
        Arad_Script table;

        if (!table.Load(data, "purchase_limit_item")) {
            return 76;  // Load error
        }

        if (table.GetRowCount() == 0) {
            return 77;  // Empty table
        }

        for (int i = 0; i < table.GetRowCount(); ++i) {
            int itemIndex = table.GetInt(i, "item_index");
            Info info;
            info.limit = table.GetInt(i, "limit");
            info.period = table.GetInt(i, "period");

            items_[itemIndex] = info;
        }

        return 0;
    }

private:
    std::map<int, Info> items_;
};

// ===== Test Data =====

const char* TEST_EXCEPTION_ITEM_DATA =
    "[material_exception_item]\n"
    "material_exception_item_index\n"
    "12345\n"
    "67890\n"
    "99999\n";

const char* TEST_PURCHASE_LIMIT_DATA =
    "[purchase_limit_item]\n"
    "item_index    limit    period\n"
    "100    5    7\n"
    "200    10    30\n";

const char* TEST_FILE_LIST_DATA =
    "file1.txt\n"
    "file2.txt\n"
    "subdir/file3.txt\n";

// ===== Basic Tests =====

TEST(Constructor_Creates_Loader) {
    ExceptionItem loader;
    ASSERT_EQ(loader.GetCount(), 0);
}

TEST(ScriptFileList_Constructor) {
    STScriptFileList list;
    ASSERT_EQ(list.GetCount(), 0);
}

TEST(ScriptFileList_Add_File) {
    STScriptFileList list;
    list.Add("test.txt");
    ASSERT_EQ(list.GetCount(), 1);
    ASSERT_STR_EQ(list.GetFile(0).c_str(), "test.txt");
}

TEST(ScriptFileList_Clear) {
    STScriptFileList list;
    list.Add("file1.txt");
    list.Add("file2.txt");
    ASSERT_EQ(list.GetCount(), 2);

    list.Clear();
    ASSERT_EQ(list.GetCount(), 0);
}

// ===== Parsing Tests (With Mock Data) =====

TEST(ExceptionItem_Parsing_Success) {
    ExceptionItem loader;

    // Mock parsing by calling parsing() directly
    int result = loader.parsing(TEST_EXCEPTION_ITEM_DATA);

    ASSERT_EQ(result, 0);
    ASSERT_EQ(loader.GetCount(), 3);
    ASSERT(loader.find_index(12345));
    ASSERT(loader.find_index(67890));
    ASSERT(loader.find_index(99999));
    ASSERT(!loader.find_index(11111));
}

TEST(ExceptionItem_Parsing_Empty_Table) {
    ExceptionItem loader;

    const char* emptyData =
        "[material_exception_item]\n"
        "material_exception_item_index\n";

    int result = loader.parsing(emptyData);

    ASSERT_EQ(result, 42);  // Empty table error
}

TEST(ExceptionItem_Parsing_Invalid_Section) {
    ExceptionItem loader;

    const char* invalidData =
        "[wrong_section]\n"
        "column\n"
        "data\n";

    int result = loader.parsing(invalidData);

    ASSERT_EQ(result, 39);  // Load error
}

TEST(PurchaseLimitItem_Parsing_Success) {
    PurchaseLimitItem loader;

    int result = loader.parsing(TEST_PURCHASE_LIMIT_DATA);

    ASSERT_EQ(result, 0);

    PurchaseLimitItem::Info info;
    ASSERT(loader.find(100, &info));
    ASSERT_EQ(info.limit, 5);
    ASSERT_EQ(info.period, 7);

    ASSERT(loader.find(200, &info));
    ASSERT_EQ(info.limit, 10);
    ASSERT_EQ(info.period, 30);

    ASSERT(!loader.find(300, &info));
}

// ===== File Loading Tests =====

TEST(LoadRDARScriptFile_Null_Filename) {
    bool result = loadRDARScriptFile("Script", nullptr);
    ASSERT(!result);
}

TEST(LoadRDARScriptFile_Valid_Filename) {
    bool result = loadRDARScriptFile("Script", "test.txt");
    ASSERT(result);  // Mock always succeeds for valid filename
}

// ===== Callback Tests =====

static int g_callbackCount = 0;

static int testCallback() {
    g_callbackCount++;
    return 0;
}

TEST(Callback_Gets_Called) {
    g_callbackCount = 0;
    g_onLoadScriptFileFunction = testCallback;

    loadRDARScriptFile("Script", "test.txt");

    ASSERT_EQ(g_callbackCount, 1);

    // Reset
    g_onLoadScriptFileFunction = nullptr;
}

// ===== Integration Tests =====

TEST(Integration_ExceptionItem_Full_Workflow) {
    // This test would normally call open(), but since we're mocking,
    // we just test parsing directly

    ExceptionItem loader;

    // Parse test data
    int result = loader.parsing(TEST_EXCEPTION_ITEM_DATA);
    ASSERT_EQ(result, 0);

    // Verify loaded data
    ASSERT_EQ(loader.GetCount(), 3);
    ASSERT(loader.find_index(12345));
}

// ===== Main Test Runner =====

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Arad_ScriptLoader Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Basic tests
    run_test_Constructor_Creates_Loader();
    run_test_ScriptFileList_Constructor();
    run_test_ScriptFileList_Add_File();
    run_test_ScriptFileList_Clear();

    // Parsing tests
    run_test_ExceptionItem_Parsing_Success();
    run_test_ExceptionItem_Parsing_Empty_Table();
    run_test_ExceptionItem_Parsing_Invalid_Section();
    run_test_PurchaseLimitItem_Parsing_Success();

    // File loading tests
    run_test_LoadRDARScriptFile_Null_Filename();
    run_test_LoadRDARScriptFile_Valid_Filename();

    // Callback tests
    run_test_Callback_Gets_Called();

    // Integration tests
    run_test_Integration_ExceptionItem_Full_Workflow();

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
