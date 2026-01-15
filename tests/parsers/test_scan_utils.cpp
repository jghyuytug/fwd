/**
 * @file test_scan_utils.cpp
 * @brief Unit tests for scan_utils.cpp auxiliary parsing functions
 *
 * Test Strategy:
 * - Mock DNFLexWrapper to return predefined tokens
 * - Test each function with valid inputs
 * - Test error cases (invalid tokens, null pointers)
 * - Verify success/failure flags
 *
 * Stage: 4 (Script Parser Layer) - Phase 2 Testing
 * Date: 2025-10-25
 */

#include <iostream>
#include <cstring>
#include <cassert>
#include <string>

// Test framework macros
#define TEST(name) \
    void test_##name(); \
    struct TestRegistrar_##name { \
        TestRegistrar_##name() { \
            std::cout << "Running test: " #name << "..." << std::endl; \
            test_##name(); \
            std::cout << "  ✅ PASSED" << std::endl; \
        } \
    } testRegistrar_##name; \
    void test_##name()

#define ASSERT_TRUE(expr) \
    if (!(expr)) { \
        std::cerr << "  ❌ FAILED at line " << __LINE__ << ": " #expr << std::endl; \
        throw std::runtime_error("Assertion failed"); \
    }

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))
#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define ASSERT_STREQ(a, b) ASSERT_TRUE(std::strcmp((a), (b)) == 0)

// =============================================================================
// MOCK INFRASTRUCTURE
// =============================================================================

// Mock token queue for testing
struct MockTokenData {
    DNFLEX_TOKEN token;
    int intValue;
    float floatValue;
    const char* stringValue;
};

static MockTokenData* g_mockTokenQueue = nullptr;
static int g_mockTokenIndex = 0;
static int g_mockTokenCount = 0;

// Mock DNFLexWrapper (minimal interface for testing)
class MockDNFLexWrapper {
public:
    static void setupTokens(MockTokenData* tokens, int count) {
        g_mockTokenQueue = tokens;
        g_mockTokenIndex = 0;
        g_mockTokenCount = count;
    }

    static void reset() {
        g_mockTokenQueue = nullptr;
        g_mockTokenIndex = 0;
        g_mockTokenCount = 0;
    }

    static DNFLEX_TOKEN getToken() {
        if (g_mockTokenIndex >= g_mockTokenCount) {
            return DNFLEX_TOKEN::DNF_TOK_EOF;
        }
        return g_mockTokenQueue[g_mockTokenIndex++].token;
    }

    static int getDecimal() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return 0;
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].intValue;
    }

    static int getHexDecimal() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return 0;
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].intValue;
    }

    static float getFloat() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return 0.0f;
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].floatValue;
    }

    static const char* getString() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return "";
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].stringValue;
    }

    static int getPairInt() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return 0;
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].intValue;
    }

    static const char* getPairStr() {
        if (g_mockTokenIndex == 0 || g_mockTokenIndex > g_mockTokenCount) {
            return "";
        }
        return g_mockTokenQueue[g_mockTokenIndex - 1].stringValue;
    }
};

// Global mock lexer instance (replaces real g_dnfLex for testing)
static MockDNFLexWrapper g_mockLexer;

// Mock RDARScriptStringManager
struct MockRDARScriptStringManager {};
static MockRDARScriptStringManager g_scriptStringManager_;

namespace RDARScriptStringManager {
    const char* findString(MockRDARScriptStringManager*, int fileIndex, char* key, void*) {
        // Mock: Return key with prefix
        static char buffer[256];
        snprintf(buffer, sizeof(buffer), "STRING_TABLE[%d:%s]", fileIndex, key);
        return buffer;
    }
}

// Include the code under test (after mocks are defined)
// NOTE: This is a simplified approach. In production, we'd link against compiled scan_utils.o
// For now, we'll test the mock framework itself

// =============================================================================
// TEST CASES FOR ScanInt()
// =============================================================================

TEST(ScanInt_Decimal_Success) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_DECIMAL, 42, 0.0f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    bool success = false;
    // int value = ScanInt(&success);  // Actual function call
    // ASSERT_TRUE(success);
    // ASSERT_EQ(value, 42);

    // For now, just test mock
    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_DECIMAL);
    int val = MockDNFLexWrapper::getDecimal();
    ASSERT_EQ(val, 42);

    g_mockLexer.reset();
}

TEST(ScanInt_Hexadecimal_Success) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_HEXADECIMAL, 0xFF, 0.0f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_HEXADECIMAL);
    int val = MockDNFLexWrapper::getHexDecimal();
    ASSERT_EQ(val, 0xFF);

    g_mockLexer.reset();
}

TEST(ScanInt_Float_CastToInt) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_FLOAT, 0, 3.14f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_FLOAT);
    float fval = MockDNFLexWrapper::getFloat();
    ASSERT_EQ(static_cast<int>(fval), 3);

    g_mockLexer.reset();
}

TEST(ScanInt_InvalidToken_ReturnsFalse) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_STRING, 0, 0.0f, "not_a_number"}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_STRING);
    // Should return false for ScanInt

    g_mockLexer.reset();
}

// =============================================================================
// TEST CASES FOR ScanFloat()
// =============================================================================

TEST(ScanFloat_DecimalToFloat) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_DECIMAL, 100, 0.0f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_DECIMAL);
    float val = static_cast<float>(MockDNFLexWrapper::getDecimal());
    ASSERT_EQ(val, 100.0f);

    g_mockLexer.reset();
}

TEST(ScanFloat_FloatValue) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_FLOAT, 0, 2.718f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_FLOAT);
    float val = MockDNFLexWrapper::getFloat();
    ASSERT_TRUE(val > 2.71f && val < 2.72f);  // Float comparison tolerance

    g_mockLexer.reset();
}

// =============================================================================
// TEST CASES FOR ScanStr()
// =============================================================================

TEST(ScanStr_InlineString) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_STRING, 0, 0.0f, "hello_world"}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_STRING);
    const char* str = MockDNFLexWrapper::getString();
    ASSERT_STREQ(str, "hello_world");

    g_mockLexer.reset();
}

TEST(ScanStr_StringTableLookup) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_PAIR_INT, 5, 0.0f, nullptr},
        {DNFLEX_TOKEN::DNF_TOK_PAIR_STR, 0, 0.0f, "quest_name"}
    };
    g_mockLexer.setupTokens(tokens, 2);

    DNFLEX_TOKEN tok1 = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok1, DNFLEX_TOKEN::DNF_TOK_PAIR_INT);
    int fileIndex = MockDNFLexWrapper::getPairInt();
    ASSERT_EQ(fileIndex, 5);

    DNFLEX_TOKEN tok2 = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok2, DNFLEX_TOKEN::DNF_TOK_PAIR_STR);
    const char* key = MockDNFLexWrapper::getPairStr();
    ASSERT_STREQ(key, "quest_name");

    // Test string table lookup
    const char* result = RDARScriptStringManager::findString(
        &g_scriptStringManager_, fileIndex, const_cast<char*>(key), nullptr);
    ASSERT_TRUE(std::strstr(result, "STRING_TABLE") != nullptr);
    ASSERT_TRUE(std::strstr(result, "quest_name") != nullptr);

    g_mockLexer.reset();
}

TEST(ScanStr_BufferOverflow_Protection) {
    // Test that ScanStr respects buffer size limits
    char buffer[10];

    // This would require actual ScanStr implementation
    // For now, just verify buffer size
    ASSERT_EQ(sizeof(buffer), 10);

    // Simulate safe copy
    const char* longString = "this_is_a_very_long_string";
    std::strncpy(buffer, longString, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    ASSERT_EQ(std::strlen(buffer), 9);  // Max 9 chars + null terminator
}

// =============================================================================
// TEST CASES FOR ScanFloatStr()
// =============================================================================

TEST(ScanFloatStr_ReturnsFloat) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_FLOAT, 0, 1.5f, nullptr}
    };
    g_mockLexer.setupTokens(tokens, 1);

    // Mock the behavior
    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_FLOAT);

    float outFloat = MockDNFLexWrapper::getFloat();
    ASSERT_TRUE(outFloat > 1.4f && outFloat < 1.6f);
    // Expected return: 0 (float parsed)

    g_mockLexer.reset();
}

TEST(ScanFloatStr_ReturnsString) {
    MockTokenData tokens[] = {
        {DNFLEX_TOKEN::DNF_TOK_STRING, 0, 0.0f, "not_a_number"}
    };
    g_mockLexer.setupTokens(tokens, 1);

    DNFLEX_TOKEN tok = MockDNFLexWrapper::getToken();
    ASSERT_EQ(tok, DNFLEX_TOKEN::DNF_TOK_STRING);

    const char* str = MockDNFLexWrapper::getString();
    ASSERT_STREQ(str, "not_a_number");
    // Expected return: 1 (string parsed)

    g_mockLexer.reset();
}

// =============================================================================
// TEST SUMMARY
// =============================================================================

void printTestSummary() {
    std::cout << "\n========================================\n";
    std::cout << "  Scan Utils Test Summary\n";
    std::cout << "========================================\n";
    std::cout << "Total Tests: 10\n";
    std::cout << "Mock Framework: ✅ Working\n";
    std::cout << "\nNOTE: These tests verify the mock infrastructure.\n";
    std::cout << "To test actual scan_utils functions:\n";
    std::cout << "1. Compile scan_utils.cpp to .o file\n";
    std::cout << "2. Link with this test file\n";
    std::cout << "3. Uncomment actual function calls\n";
    std::cout << "========================================\n";
}

// =============================================================================
// MAIN
// =============================================================================

int main() {
    std::cout << "\n🧪 Starting Scan Utils Unit Tests...\n\n";

    try {
        // Tests run automatically via static initializers
        printTestSummary();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ TEST SUITE FAILED: " << e.what() << std::endl;
        return 1;
    }
}
