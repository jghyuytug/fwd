#ifndef LEXER_DNFLEX_WRAPPER_H
#define LEXER_DNFLEX_WRAPPER_H

/**
 * @file dnflex_wrapper.h
 * @brief DNF Lexer Wrapper - Unified Token Dispatcher
 *
 * Original source: df_game_r_part24.c:34188-34900
 * Extraction date: 2025-10-23
 *
 * DNFLexWrapper provides a unified interface for both normal text scripts
 * and compiled binary scripts. It manages DNFLex and CompiledDNFLex instances
 * and dispatches to the appropriate lexer based on file type.
 */

#include <string>
#include "lexer/token_types.h"
#include "lexer/dnflex.h"
#include "lexer/compiled_dnflex.h"

// TCHAR compatibility (same as dnflex.h)
#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
typedef wchar_t TCHAR;
#else
typedef std::string tstring;
typedef char TCHAR;
#endif

/**
 * @class DNFLexWrapper
 * @brief Unified interface for DNF script lexical analysis
 *
 * Manages two types of lexers:
 * - DNFLex: For text scripts (.stg files)
 * - CompiledDNFLex: For binary scripts (.bcsf files)
 *
 * Automatically detects file type (magic: 0xD0B0 for compiled)
 * and dispatches to appropriate lexer.
 *
 * Source: df_game_r_part24.c:34188-34900
 * Virtual table: off_8E2EC30
 */
class DNFLexWrapper {
public:
    // ========================================================================
    // Nested Types
    // ========================================================================

    /**
     * @enum ENUM_LEX_TYPE
     * @brief Lexer mode selection
     */
    enum ENUM_LEX_TYPE {
        LEX_TYPE_NORMAL = 0,     ///< Normal text script (use DNFLex)
        LEX_TYPE_COMPILED = 1    ///< Compiled binary script (use CompiledDNFLex)
    };

    /**
     * @typedef TLexerError
     * @brief Error callback function type
     */
    typedef void (*TLexerError)(const char* msg);

    /**
     * @typedef TLoadStream
     * @brief Stream loader callback type
     */
    typedef bool (*TLoadStream)(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    /**
     * @typedef TIsDbcsLeadChar
     * @brief DBCS lead byte detector callback type
     */
    typedef int (*TIsDbcsLeadChar)(unsigned char ch);

    /**
     * @typedef TLexerOutput
     * @brief Lexer output callback type
     */
    typedef void (*TLexerOutput)(const char* buf, int size);

    // ========================================================================
    // Part 2: Token Access Methods (Token 访问方法)
    // ========================================================================

    /**
     * @brief Get next token (main dispatcher)
     * @return Token type
     * Source: part24.c:34516 ⭐ Core method
     */
    DNFLEX_TOKEN getToken();

    /**
     * @brief Get last token type
     * @return Last token
     * Source: part24.c:34320
     */
    DNFLEX_TOKEN getLastToken();

    /**
     * @brief Get last token data as raw pointer
     * @return Pointer to token text
     * Source: part24.c:34326
     */
    const char* getLastTokenData();

    /**
     * @brief Get last token data as std::string
     * @param retstr Output string
     * @return Pointer to retstr
     * Source: part24.c:34363
     */
    tstring* getLastTokenDataAsString(tstring* retstr);

    /**
     * @brief Get last token line number
     * @return Line number
     * Source: part24.c:34348
     */
    int getLastLineNo();

    /**
     * @brief Get integer/decimal value
     * @return Integer value
     * Source: part24.c:34215
     */
    int getDecimal();

    /**
     * @brief Get hexadecimal value
     * @return Hexadecimal integer
     * Source: part24.c:34491
     */
    int getHexDecimal();

    /**
     * @brief Get floating-point value
     * @return Float value
     * Source: part24.c:34472
     */
    float getFloat();

    /**
     * @brief Get integer value (alias)
     * @return Integer value
     * Note: May be same as getDecimal()
     */
    int getInt() { return getDecimal(); }

    // ========================================================================
    // Part 3: String Access Methods (String 访问方法)
    // ========================================================================

    /**
     * @brief Get string value
     * @return String pointer
     * Source: part24.c:34457
     */
    const char* getString();

    /**
     * @brief Get type token value
     * @return Type string
     * Source: part24.c:34275
     */
    const char* getType();

    /**
     * @brief Get custom data value
     * @return Custom data string
     * Source: part24.c:34290
     */
    const char* getCustomData();

    /**
     * @brief Get connector token
     * @return Connector string
     * Source: part24.c:34305
     */
    const char* getConnector();

    /**
     * @brief Get pair string component
     * @return String key from pair [int "str"]
     * Source: part24.c:34410
     */
    const char* getPairStr();

    /**
     * @brief Get pair integer component
     * @return Integer key from pair [int "str"]
     * Source: part24.c:34434
     */
    int getPairInt();

    // ========================================================================
    // Part 4: Stream Management Methods (Stream 管理方法)
    // ========================================================================

    /**
     * @brief Switch to new input stream
     * @param dir Directory path
     * @param filename File name
     * @param s Stream data (NULL = load via callback)
     * @return true if switched successfully
     * Source: part24.c:34645
     */
    bool switchInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);

    /**
     * @brief Close all input streams
     * Source: part24.c:34638
     */
    void closeInputStream();

    /**
     * @brief Get current input stream
     * @return Stream pointer
     * Source: part24.c:34565
     */
    std::istream* getInputStream();

    /**
     * @brief Get current input stream size
     * @return Stream size in bytes
     * Source: part24.c:34576
     */
    int getInputStreamSize();

    /**
     * @brief Get current input stream buffer
     * @return Buffer pointer
     * Source: part24.c:34591
     */
    char* getInputStreamBuffer();

    /**
     * @brief Get current stream name
     * @param retstr Output string
     * @return Pointer to retstr
     * Source: part24.c:34554
     */
    tstring* getStreamName(tstring* retstr);

    /**
     * @brief Set file stream data
     * @param s Stream data output
     * @param path File path
     * @return true if loaded successfully
     * Source: part24.c:34756
     */
    bool setFileStreamData(stream_data_t* s, const TCHAR* path);

    /**
     * @brief Set string stream data (from buffer)
     * @param s Stream data output
     * @param buffer Input buffer
     * @param bufferSize Buffer size
     * @return true if set successfully
     * Source: part24.c:34606
     */
    bool setStringStreamData(stream_data_t* s, char* buffer, unsigned int bufferSize);

    // ========================================================================
    // Part 5: Configuration and Lifecycle (配置和生命周期)
    // ========================================================================

    /**
     * @brief Constructor
     * Source: part24.c:34742
     */
    DNFLexWrapper();

    /**
     * @brief Destructor
     * Source: part24.c:34725
     */
    virtual ~DNFLexWrapper();

    /**
     * @brief Set lexer type
     * @param lexType LEX_TYPE_NORMAL or LEX_TYPE_COMPILED
     * Source: part24.c:34188
     */
    void setLexType(ENUM_LEX_TYPE lexType);

    /**
     * @brief Get current lexer type
     * @return Current lexer type
     * Source: part24.c:34194
     */
    ENUM_LEX_TYPE getLexType();

    /**
     * @brief Set callback handlers
     * @param IsDbcsLeadCharFunc DBCS detector (unused in wrapper)
     * @param LexerOutputFunc Output handler (unused in wrapper)
     * @param LexerErrorFunc Error handler
     * @param LoadStreamFunc Stream loader
     * Source: part24.c:34710
     */
    void SetHandler(
        DNFLex::TIsDbcsLeadChar IsDbcsLeadCharFunc,
        DNFLex::TLexerOutput LexerOutputFunc,
        TLexerError LexerErrorFunc,
        TLoadStream LoadStreamFunc);

    /**
     * @brief Set string table from file (for CompiledDNFLex)
     * @param fileName String table file path
     * @return true if loaded successfully
     * Source: part24.c:34392
     */
    bool setStringTableFromFile(const TCHAR* fileName);

    /**
     * @brief Set string table from buffer (for CompiledDNFLex)
     * @param buffer Buffer containing string table
     * @param size Buffer size
     * @return true if loaded successfully
     * Source: part24.c:34401
     */
    bool setStringTableFromBuffer(const unsigned char* buffer, int size);

    /**
     * @brief Load stream callback dispatcher
     * @param dir Directory
     * @param filename File name
     * @param s Output stream data
     * @return true if loaded
     * Source: part24.c:34200
     */
    bool LoadStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s);


protected:
    // ========================================================================
    // Part 6: Member Variables (成员变量)
    // ========================================================================

    // Embedded lexer instances
    DNFLex dnfLex_;                  ///< Normal text lexer (LEX_TYPE_NORMAL)
    CompiledDNFLex compiledDnfLex_;  ///< Binary script lexer (LEX_TYPE_COMPILED)

    // Lexer mode
    ENUM_LEX_TYPE lexType_;          ///< Current lexer type (NORMAL or COMPILED)

    // Cached token values
    DNFLEX_TOKEN lastToken_;         ///< Last token type
    int lastIntValue_;               ///< Cached integer value
    float lastFloatValue_;           ///< Cached float value
    const char* lastStringValue_;    ///< Cached string pointer

    // Callbacks
    TLexerError LexerError_;         ///< Error callback
    TLoadStream LoadStream_;         ///< Stream loader callback

    // Note: Virtual table pointer (_vptr_DNFLexWrapper) is managed by compiler
};

#endif // LEXER_DNFLEX_WRAPPER_H

