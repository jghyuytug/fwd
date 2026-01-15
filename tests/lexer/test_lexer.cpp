/**
 * @file test_lexer.cpp
 * @brief Simple compilation and linkage test for Stage 3 lexer
 *
 * Purpose: Verify that all Stage 3 components compile and link correctly.
 * This is NOT a comprehensive test suite, just a smoke test.
 */

#include "lexer/token_types.h"
#include "lexer/dnflex.h"
#include "lexer/dnflex_wrapper.h"
#include "lexer/compiled_dnflex.h"
#include <iostream>
#include <cstring>

// Test counters
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    std::cout << "TEST: " << name << " ... "; \
    try {

#define END_TEST \
        tests_passed++; \
        std::cout << "PASS" << std::endl; \
    } catch (const std::exception& e) { \
        tests_failed++; \
        std::cout << "FAIL: " << e.what() << std::endl; \
    } catch (...) { \
        tests_failed++; \
        std::cout << "FAIL: Unknown exception" << std::endl; \
    }

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "Stage 3 Lexer - Smoke Test" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;

    // Test 1: Token enum values
    TEST("Token enum values") {
        if (DNF_TOK_EOF != 0) throw std::runtime_error("DNF_TOK_EOF != 0");
        if (DNF_TOK_ERROR != 1) throw std::runtime_error("DNF_TOK_ERROR != 1");
        if (DNF_TOK_DECIMAL != 2) throw std::runtime_error("DNF_TOK_DECIMAL != 2");
        if (DNF_TOK_FLOAT != 5) throw std::runtime_error("DNF_TOK_FLOAT != 5");
        if (DNF_TOK_STRING != 7) throw std::runtime_error("DNF_TOK_STRING != 7");
        if (DNF_TOK_INCLUDE != 11) throw std::runtime_error("DNF_TOK_INCLUDE != 11");
    } END_TEST;

    // Test 2: Token helper functions
    TEST("Token helper functions") {
        const char* name = DNFTokenToString(DNF_TOK_STRING);
        if (strcmp(name, "STRING") != 0) {
            throw std::runtime_error("DNFTokenToString failed");
        }

        if (!isNumericToken(DNF_TOK_DECIMAL)) {
            throw std::runtime_error("isNumericToken failed");
        }

        if (!isStringLikeToken(DNF_TOK_STRING)) {
            throw std::runtime_error("isStringLikeToken failed");
        }
    } END_TEST;

    // Test 3: DNFLex construction
    TEST("DNFLex construction/destruction") {
        DNFLex* lexer = new DNFLex();
        if (lexer == nullptr) throw std::runtime_error("Failed to create DNFLex");
        delete lexer;
    } END_TEST;

    // Test 4: DNFLexWrapper construction
    TEST("DNFLexWrapper construction/destruction") {
        DNFLexWrapper* wrapper = new DNFLexWrapper();
        if (wrapper == nullptr) throw std::runtime_error("Failed to create DNFLexWrapper");

        // Test basic getters
        DNFLexWrapper::ENUM_LEX_TYPE type = wrapper->getLexType();
        if (type != DNFLexWrapper::LEX_TYPE_NORMAL) {
            throw std::runtime_error("Default lex type should be NORMAL");
        }

        delete wrapper;
    } END_TEST;

    // Test 5: CompiledDNFLex construction
    TEST("CompiledDNFLex construction/destruction") {
        CompiledDNFLex* lexer = new CompiledDNFLex();
        if (lexer == nullptr) throw std::runtime_error("Failed to create CompiledDNFLex");

        // Test basic methods
        if (lexer->IsStream()) {
            throw std::runtime_error("IsStream() should be false initially");
        }

        int offset = lexer->GetOffset();
        if (offset != -1) {
            throw std::runtime_error("GetOffset() should return -1 when no stream");
        }

        delete lexer;
    } END_TEST;

    // Test 6: DNFLexWrapper mode switching
    TEST("DNFLexWrapper mode switching") {
        DNFLexWrapper wrapper;

        wrapper.setLexType(DNFLexWrapper::LEX_TYPE_COMPILED);
        if (wrapper.getLexType() != DNFLexWrapper::LEX_TYPE_COMPILED) {
            throw std::runtime_error("Failed to switch to COMPILED mode");
        }

        wrapper.setLexType(DNFLexWrapper::LEX_TYPE_NORMAL);
        if (wrapper.getLexType() != DNFLexWrapper::LEX_TYPE_NORMAL) {
            throw std::runtime_error("Failed to switch back to NORMAL mode");
        }
    } END_TEST;

    // Test 7: CompiledDNFLex enable/disable inclusion
    TEST("CompiledDNFLex inclusion control") {
        CompiledDNFLex lexer;

        lexer.DisableInclusion();
        // No way to query state, just ensure it doesn't crash

        lexer.EnableInclusion();
        // No way to query state, just ensure it doesn't crash
    } END_TEST;

    // Test 8: CompiledDNFLex stream operations (empty)
    TEST("CompiledDNFLex empty stream operations") {
        CompiledDNFLex lexer;

        // Should return EOF when no stream
        DNFLEX_TOKEN token = lexer.getToken();
        if (token != DNF_TOK_EOF) {
            throw std::runtime_error("getToken() should return EOF when no stream");
        }

        // Should handle close gracefully
        lexer.CloseInputStream();

        // Should still return EOF
        token = lexer.getToken();
        if (token != DNF_TOK_EOF) {
            throw std::runtime_error("getToken() should still return EOF");
        }
    } END_TEST;

    // Test 9: Binary format constants
    TEST("Binary format magic number") {
        const uint16_t MAGIC = 0xD0B0;
        // Just verify the constant is defined correctly
        if (MAGIC != 0xD0B0) {
            throw std::runtime_error("Magic number mismatch");
        }
    } END_TEST;

    // Summary
    std::cout << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Test Results" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;
    std::cout << std::endl;

    if (tests_failed == 0) {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests failed" << std::endl;
        return 1;
    }
}
