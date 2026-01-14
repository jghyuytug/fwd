/**
 * @file flexlexer_stub.h
 * @brief Stub for FlexLexer base class
 *
 * Minimal stub to allow parser compilation without full Flex library.
 * In production, this would be provided by the Flex library.
 */

#ifndef LEXER_FLEXLEXER_STUB_H
#define LEXER_FLEXLEXER_STUB_H

#include <iostream>

// Forward declaration
struct yy_buffer_state;

/**
 * @class __dnf_script__FlexLexer
 * @brief Stub for Flex lexer base class
 */
class __dnf_script__FlexLexer {
public:
    __dnf_script__FlexLexer() : yylineno(1), yy_start(1), yytext(nullptr) {}
    __dnf_script__FlexLexer(std::istream*, std::ostream*) : yylineno(1), yy_start(1), yytext(nullptr) {}
    virtual ~__dnf_script__FlexLexer() {}

    // Virtual methods that DNFLex will override
    virtual int yylex() = 0;

    // Flex-generated members (public for wrapper access)
    int yylineno;        ///< Current line number
    int yy_start;        ///< Start condition
    char* yytext;        ///< Matched text buffer

    // Flex buffer management methods (stubs)
    virtual yy_buffer_state* yy_create_buffer(std::istream*, int) { return nullptr; }
    virtual void yypush_buffer_state(yy_buffer_state*) {}
    virtual void yypop_buffer_state() {}
};

#endif // LEXER_FLEXLEXER_STUB_H
