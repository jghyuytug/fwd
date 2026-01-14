#ifndef LEXER_TOKEN_TYPES_H
#define LEXER_TOKEN_TYPES_H

/**
 * @file token_types.h
 * @brief DNF Script Lexer Token Type Definitions
 *
 * This file defines all token types recognized by the DNF script lexical analyzer.
 * The token values are reconstructed from usage patterns in the decompiled source code.
 *
 * Original location: Inferred from df_game_r_part20.c, df_game_r_part24.c usage
 * Reconstruction date: 2025-10-23
 *
 * Token values were determined by analyzing:
 * - Code comparisons: (result - 2) <= 1, result == 5, result == 7, result == 11
 * - Switch case statements in df_game_r_part20.c (Scan functions)
 * - Binary format token storage in df_game_r_part24.c (CompiledDNFLex::getToken)
 *
 * Confidence levels:
 * - Confirmed: Value explicitly used in code comparisons
 * - 🔍 Inferred: Value derived from code logic and token ordering
 * - Estimated: Value based on naming patterns and gaps
 */

/**
 * @enum DNFLEX_TOKEN
 * @brief Token types for DNF script lexical analysis
 *
 * This enum defines all token types that can be returned by the DNFLex and
 * CompiledDNFLex lexical analyzers. Tokens represent syntactic elements in
 * DNF script files (.stg, .bcsf, etc.).
 *
 * Token values are stored as single bytes in compiled binary scripts (.bcsf),
 * so all values must fit in the range 0-255.
 */
enum DNFLEX_TOKEN {
    /**
     * @brief End of file token
     *
     * Returned when the lexer reaches the end of the input stream.
     * This token signals that no more tokens are available.
     *
     * Value: 0 Confirmed (used in: DNFLex::get_next_token line 33852)
     * Usage: Stream termination, end of script file
     */
    DNF_TOK_EOF = 0,

    /**
     * @brief Error token
     *
     * Returned when the lexer encounters a syntax error or invalid input.
     * Error details are typically logged via LexerError callback.
     *
     * Value: 1 Confirmed (from: (unsigned int)result > DNFLEX_TOKEN::DNF_TOK_ERROR)
     * Usage: Syntax errors, malformed tokens, invalid characters
     */
    DNF_TOK_ERROR = 1,

    /**
     * @brief Decimal integer token
     *
     * Represents a decimal integer literal: 123, -456, +789
     * Parsed via DNFLexWrapper::getDecimal() or DNFLexWrapper::getInt()
     *
     * Value: 2 Confirmed (from: (unsigned int)(result - 2) <= 1)
     * Usage: Integer values, counts, IDs, numeric constants
     * Example: skill_level = 10;
     */
    DNF_TOK_DECIMAL = 2,

    /**
     * @brief Hexadecimal integer token
     *
     * Represents a hexadecimal integer literal: 0x1A2B, 0XFFFF
     * Parsed via DNFLexWrapper::getHexDecimal()
     *
     * Value: 3 Confirmed (from: (unsigned int)(result - 2) <= 1, consecutive with DECIMAL)
     * Usage: Color codes, bit flags, memory addresses
     * Example: color = 0xFF0000;
     */
    DNF_TOK_HEXADECIMAL = 3,

    /**
     * @brief Reserved / unused token slot
     *
     * This token value appears to be unused in the current codebase.
     * May be reserved for future use or deprecated functionality.
     *
     * Value: 4 Estimated (gap between HEXADECIMAL and FLOAT)
     */
    DNF_TOK_RESERVED_4 = 4,

    /**
     * @brief Floating-point number token
     *
     * Represents a floating-point literal: 3.14, -0.5, 1.0
     * Parsed via DNFLexWrapper::getFloat()
     *
     * Value: 5 Confirmed (used in: part24.c:34540, part20.c:79432)
     * Usage: Damage multipliers, scaling factors, percentages
     * Example: damage_rate = 1.5;
     */
    DNF_TOK_FLOAT = 5,

    /**
     * @brief Type identifier token
     *
     * Represents a type or class name: `fighter`, `mage`, `item`
     * Type tokens are typically enclosed in backticks in source scripts.
     * Parsed via DNFLexWrapper::getType()
     *
     * Value: 6 🔍 Inferred (based on usage patterns in part20.c)
     * Usage: Class types, item types, skill types, enum values
     * Example: class_type = `fighter`;
     */
    DNF_TOK_TYPE = 6,

    /**
     * @brief String literal token
     *
     * Represents a string literal: "hello", "skill_name.stg"
     * Strings are enclosed in double quotes in source scripts.
     * Parsed via DNFLexWrapper::getString() or DNFLex::tok_str()
     *
     * Value: 7 Confirmed (used in: part24.c:36488, part20.c:78485)
     * Usage: File paths, descriptions, text data, string constants
     * Example: file = "skill/fighter.stg";
     */
    DNF_TOK_STRING = 7,

    /**
     * @brief Pair integer component token
     *
     * Represents the integer part of a pair token: [123 "key"]
     * Pairs are used for internationalization and data lookup.
     * The integer is used as a key ID in RDARScriptStringManager.
     *
     * Value: 8 🔍 Inferred (from pair usage patterns)
     * Usage: Localized strings, data table lookups
     * Format: [int_key "string_key"]
     * Example: name = [1001 "item_sword"];
     */
    DNF_TOK_PAIR_INT = 8,

    /**
     * @brief Pair string component token
     *
     * Represents the string part of a pair token: [123 "key"]
     * This token immediately follows DNF_TOK_PAIR_INT in pairs.
     * Both components are used together for string table lookup.
     *
     * Value: 9 🔍 Inferred (follows PAIR_INT)
     * Usage: Second part of pair tokens
     * Format: [int_key "string_key"]
     * Note: Some code treats full pairs as "STRING" after lookup
     */
    DNF_TOK_PAIR_STR = 9,

    /**
     * @brief Custom data token
     *
     * Represents custom or extended data types not covered by other tokens.
     * Used for special data formats or extensions.
     *
     * Value: 10 🔍 Inferred (from usage in part20.c:79452, 81264)
     * Usage: Extended data types, plugin data, custom formats
     */
    DNF_TOK_CUSTOM_DATA = 10,

    /**
     * @brief Include directive token
     *
     * Represents a #include directive for file inclusion.
     * Triggers loading of another script file into the current parse context.
     * Must be followed by a STRING token containing the file path.
     *
     * Value: 11 Confirmed (used in: part24.c:33859, 36474)
     * Usage: File inclusion, script composition
     * Example: #include "common.stg"
     */
    DNF_TOK_INCLUDE = 11,

    /**
     * @brief Connector token (optional)
     *
     * Represents connectors or operators: ->, ::, etc.
     * May be used for path separators or namespace resolution.
     *
     * Value: 12 Estimated (referenced in DNFLexWrapper::getConnector)
     * Usage: Path separators, operators, special symbols
     * Note: Exact value uncertain, may not be used
     */
    DNF_TOK_CONNECTOR = 12,

    /**
     * @brief Maximum token value marker
     *
     * Not a real token - used to mark the end of the enum.
     * Useful for bounds checking and iteration.
     */
    DNF_TOK_MAX = 13
};

/**
 * @brief Type alias for DNFLEX_TOKEN enum
 *
 * Provides a shorter name for the token type in function signatures.
 */
typedef enum DNFLEX_TOKEN DNFLEX_TOKEN;

/**
 * @brief Convert token type to string name
 * @param token Token type value
 * @return String name of the token (e.g., "DNF_TOK_STRING")
 *
 * Useful for debugging and error messages.
 */
inline const char* DNFTokenToString(DNFLEX_TOKEN token) {
    switch (token) {
        case DNF_TOK_EOF: return "DNF_TOK_EOF";
        case DNF_TOK_ERROR: return "DNF_TOK_ERROR";
        case DNF_TOK_DECIMAL: return "DNF_TOK_DECIMAL";
        case DNF_TOK_HEXADECIMAL: return "DNF_TOK_HEXADECIMAL";
        case DNF_TOK_RESERVED_4: return "DNF_TOK_RESERVED_4";
        case DNF_TOK_FLOAT: return "DNF_TOK_FLOAT";
        case DNF_TOK_TYPE: return "DNF_TOK_TYPE";
        case DNF_TOK_STRING: return "DNF_TOK_STRING";
        case DNF_TOK_PAIR_INT: return "DNF_TOK_PAIR_INT";
        case DNF_TOK_PAIR_STR: return "DNF_TOK_PAIR_STR";
        case DNF_TOK_CUSTOM_DATA: return "DNF_TOK_CUSTOM_DATA";
        case DNF_TOK_INCLUDE: return "DNF_TOK_INCLUDE";
        case DNF_TOK_CONNECTOR: return "DNF_TOK_CONNECTOR";
        case DNF_TOK_MAX: return "DNF_TOK_MAX";
        default: return "DNF_TOK_UNKNOWN";
    }
}

/**
 * @brief Check if token represents a numeric value
 * @param token Token type to check
 * @return true if token is DECIMAL, HEXADECIMAL, or FLOAT
 */
inline bool isNumericToken(DNFLEX_TOKEN token) {
    return token == DNF_TOK_DECIMAL ||
           token == DNF_TOK_HEXADECIMAL ||
           token == DNF_TOK_FLOAT;
}

/**
 * @brief Check if token represents a string-like value
 * @param token Token type to check
 * @return true if token is STRING, TYPE, or CUSTOM_DATA
 */
inline bool isStringLikeToken(DNFLEX_TOKEN token) {
    return token == DNF_TOK_STRING ||
           token == DNF_TOK_TYPE ||
           token == DNF_TOK_CUSTOM_DATA;
}

/**
 * @brief Check if token is part of a pair
 * @param token Token type to check
 * @return true if token is PAIR_INT or PAIR_STR
 */
inline bool isPairToken(DNFLEX_TOKEN token) {
    return token == DNF_TOK_PAIR_INT ||
           token == DNF_TOK_PAIR_STR;
}

#endif // LEXER_TOKEN_TYPES_H
