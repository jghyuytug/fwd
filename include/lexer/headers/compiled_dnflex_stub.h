/**
 * @file compiled_dnflex_stub.h
 * @brief Stub for CompiledDNFLex class
 *
 * Minimal stub to allow DNFLexWrapper compilation without full binary lexer implementation.
 * In production, this would be the full compiled script lexer.
 */

#ifndef LEXER_COMPILED_DNFLEX_STUB_H
#define LEXER_COMPILED_DNFLEX_STUB_H

/**
 * @class CompiledDNFLex
 * @brief Stub for binary script lexer
 *
 * This is a placeholder for the compiled (.bcsf) script lexer.
 * For validation purposes, we only need text script (.stg) parsing.
 */
class CompiledDNFLex {
public:
    CompiledDNFLex() {}
    ~CompiledDNFLex() {}

    // Stub methods - minimal interface
    int getToken() { return 0; }
    bool isActive() { return false; }
};

#endif // LEXER_COMPILED_DNFLEX_STUB_H
