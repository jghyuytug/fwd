/**
 * @file dnflex_wrapper_stub.cpp
 * @brief Stub implementation of DNFLexWrapper for validation
 *
 * This is a minimal stub implementation that provides empty methods
 * to allow linking unit tests. In production, this would be the full
 * lexer implementation from lexer/dnflex_wrapper.cpp.
 */

#include "lexer/dnflex_wrapper.h"
#include <cstring>

// ============================================================================
// Constructor and Destructor
// ============================================================================

DNFLexWrapper::DNFLexWrapper() {
    // Stub: Initialize to default state
}

DNFLexWrapper::~DNFLexWrapper() {
    // Stub: Cleanup
}

// ============================================================================
// Token Retrieval Methods (stubs returning dummy values)
// ============================================================================

DNFLEX_TOKEN DNFLexWrapper::getToken() {
    // Stub: Always return end-of-file token
    return DNF_TOK_EOF;
}

int DNFLexWrapper::getDecimal() {
    // Stub: Return dummy integer
    return 0;
}

int DNFLexWrapper::getHexDecimal() {
    // Stub: Return dummy integer
    return 0;
}

float DNFLexWrapper::getFloat() {
    // Stub: Return dummy float
    return 0.0f;
}

const char* DNFLexWrapper::getString() {
    // Stub: Return empty string
    return "";
}

const char* DNFLexWrapper::getType() {
    // Stub: Return empty type
    return "";
}

int DNFLexWrapper::getPairInt() {
    // Stub: Return dummy integer
    return 0;
}

const char* DNFLexWrapper::getPairStr() {
    // Stub: Return empty string
    return "";
}

// ============================================================================
// Additional Methods (if referenced)
// ============================================================================

bool DNFLexWrapper::setFileStreamData(stream_data_t* s, const TCHAR* path) {
    // Stub: Always succeed
    return true;
}

bool DNFLexWrapper::setStringStreamData(stream_data_t* s, char* buffer, unsigned int bufferSize) {
    // Stub: Always succeed
    return true;
}

bool DNFLexWrapper::switchInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s) {
    // Stub: Always succeed
    return true;
}
