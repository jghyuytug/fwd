#ifndef LEXER_DNFLEX_H
#define LEXER_DNFLEX_H

/**
 * @file dnflex.h
 * @brief DNF Script Lexical Analyzer - Core Flex Lexer
 *
 * Original source: df_game_r_part24.c:33400-34186
 * Extraction date: 2025-10-23
 */

#include <string>
#include <vector>
#include <iostream>
#include "lexer/token_types.h"
#include "data/stream_manager.h" // stream_data_t from Stage 2
#include "lexer/flexlexer_stub.h" // FlexLexer base class stub

// Forward declarations
struct yy_buffer_state;

// TCHAR compatibility
#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
typedef wchar_t TCHAR;
#else
typedef std::string tstring;
typedef char TCHAR;
#endif

/**
 * @class DNFLex
 * @brief Core Flex-based lexical analyzer for DNF scripts
 *
 * Source: df_game_r_part24.c:33400-34186
 * Virtual table: off_8E2EBA8
 */
class DNFLex : public __dnf_script__FlexLexer {
public:
    // ========================================================================
    // Nested Types
    // ========================================================================

    /**
     * @struct stream_t
     * @brief Stream stack entry for nested file inclusion
     * Source: df_game_r_part24.c:33896 (destructor)
     */
    struct stream_t {
        std::string dir;              ///< Directory path
        std::string filename;         ///< Filename
        stream_data_t s;              ///< Stream data (from Stage 2)
        unsigned int line_number;     ///< Line number when pushed

        ~stream_t(); // Source: part24.c:33896
    };

    /**
     * @typedef TIsDbcsLeadChar
     * @brief DBCS lead byte detector callback
     */
    typedef int (*TIsDbcsLeadChar)(unsigned char ch);

    /**
     * @typedef TLexerOutput
     * @brief Lexer output callback
     */
    typedef void (*TLexerOutput)(const char* buf, int size);

    /**
     * @typedef TLexerError
     * @brief Lexer error callback
     */
    typedef void (*TLexerError)(const char* msg);

    /**
     * @typedef TLoadStream
     * @brief Stream loader callback
     */
    typedef bool (*TLoadStream)(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    // ========================================================================
    // Part 1: Pure Virtual Override (from FlexLexer base)
    // ========================================================================

    /**
     * @brief Main lexer function (required override)
     * @return Token code
     * Note: Stub implementation for validation
     */
    virtual int yylex() { return 0; }

    // ========================================================================
    // Part 2: Small Methods (简单方法，1-10行)
    // ========================================================================

    /**
     * @brief Get string value of last STRING token
     * @return Pointer to string buffer
     * Source: part24.c:33430
     */
    const char* tok_str();

    /**
     * @brief Get length of last STRING token
     * @return Length in bytes
     * Source: part24.c:33436
     */
    int tok_str_len();

    /**
     * @brief Get current line number
     * @return Line number (1-based)
     * Source: part24.c:33467
     */
    int getLineNumber();

    /**
     * @brief Disable #include directive processing
     * Source: part24.c:33473
     */
    void DisableInclusion();

    /**
     * @brief Enable #include directive processing
     * Source: part24.c:33479
     */
    void EnableInclusion();

    /**
     * @brief Check if stream stack is not empty
     * @return true if active streams exist
     * Source: part24.c:33453
     */
    bool IsStream();

    /**
     * @brief Get current input stream
     * @return Pointer to current stream_t or NULL
     * Source: part24.c:33459
     */
    stream_t* GetInputStream();

    /**
     * @brief Load stream via callback
     * @return true if loaded successfully
     * Source: part24.c:33442
     */
    bool LoadStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    /**
     * @brief Call LexerOutput callback
     * Source: part24.c:33410
     */
    void LexerOutput(const char* buf, int size);

    /**
     * @brief Call LexerError callback
     * Source: part24.c:33420
     */
    void LexerError(const char* msg);

    /**
     * @brief Close all input streams
     * Source: part24.c:33609
     */
    void CloseInputStream();

    // ========================================================================
    // Part 3: Medium Methods (中型方法，构造/析构/Stream管理)
    // ========================================================================

    /**
     * @brief Constructor
     * Initializes DNFLex with default settings
     * Source: part24.c:33512-33528
     */
    DNFLex();

    /**
     * @brief Destructor
     * Cleans up all streams and resources
     * Source: part24.c:33617-33684
     */
    virtual ~DNFLex();

    /**
     * @brief Set callback functions
     * @param IsDbcsLeadCharFunc DBCS detector
     * @param LexerOutputFunc Output handler
     * @param LexerErrorFunc Error handler
     * @param LoadStreamFunc Stream loader
     * Source: part24.c:33400-33407
     */
    void SetHandler(
        TIsDbcsLeadChar IsDbcsLeadCharFunc,
        TLexerOutput LexerOutputFunc,
        TLexerError LexerErrorFunc,
        TLoadStream LoadStreamFunc);

    /**
     * @brief Get current stream name (dir/filename)
     * @param retstr Output string
     * @return Pointer to retstr
     * Source: part24.c:33492-33507
     */
    tstring* getStreamName(tstring* retstr);

    /**
     * @brief Pop current stream from stack
     * @param pop_all If true, pop all streams
     * @return true if popped successfully
     * Source: part24.c:33547-33606
     */
    bool PopInputStream(bool pop_all);

    /**
     * @brief Switch to new input stream
     * @param dir Directory path
     * @param filename File name
     * @param s Stream data (NULL = use LoadStream callback)
     * @return true if switched successfully
     * Source: part24.c:33823-33831
     */
    bool SwitchInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    // ========================================================================
    // Part 4: Core Methods (核心方法，大型复杂方法)
    // ========================================================================

    /**
     * @brief Push new input stream onto stack
     * @param dir Directory path
     * @param filename File name
     * @param s Stream data (NULL = use LoadStream callback)
     * @return true if pushed successfully
     *
     * Adds new stream for #include directive processing.
     * Source: part24.c:33695-33820 (120 lines)
     */
    bool PushInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    /**
     * @brief Get next token from input stream
     * @return Token type (DNFLEX_TOKEN enum)
     *
     * Main entry point for lexical analysis. Calls Flex yylex() and handles
     * #include directives and EOF automatically.
     *
     * Source: part24.c:33834-33893 (60 lines)
     */
    DNFLEX_TOKEN get_next_token();

    // ========================================================================
    // Part 5: Static Helper Methods
    // ========================================================================

    /**
     * @brief Destroy stream data resources
     * @param s Stream data to clean up
     *
     * Deletes stream and buffer pointers, sets to NULL.
     * Source: part24.c:33532-33544
     */
    static void stream_data_destroy(stream_data_t* s);


protected:
    // ========================================================================
    // Part 5: Member Variables (成员变量)
    // ========================================================================

    // Stream stack
    std::vector<stream_t> stream_stack_;  ///< Stack of input streams (max ~64)

    // String token buffer
    char STR_TOK_BUF[4096];    ///< Buffer for string tokens (4KB)
    char* m_str_tok_pos;        ///< Current position in STR_TOK_BUF

    // Error message buffer
    char ERR_BUF[256];          ///< Buffer for error messages

    // Flags
    bool is_dbcs_;              ///< DBCS mode (Asian languages)
    bool allow_inclusion_;      ///< Allow #include processing (default true)

    // Callbacks
    TIsDbcsLeadChar IsDbcsLeadChar_;  ///< DBCS lead byte detector
    TLexerOutput LexerOutput_;        ///< Output callback
    TLexerError LexerError_;          ///< Error callback
    TLoadStream LoadStream_;          ///< Stream loader callback

    // Note: Flex variables (yytext, yylineno, yy_start, etc.)
    // are inherited from __dnf_script__FlexLexer base class
};

#endif // LEXER_DNFLEX_H

