/**
 * @file compiled_dnflex.h
 * @brief Compiled DNF Lexer - Binary Script Lexical Analyzer
 *
 * Lexical analyzer for compiled binary script files (.bcsf format).
 * Supports magic number 0xD0B0, string table management, and efficient token streaming.
 *
 * Original source: df_game_r_part24.c:35571-36705
 * Extraction date: 2025-10-24
 */

#ifndef COMPILED_DNFLEX_H
#define COMPILED_DNFLEX_H

#include "lexer/token_types.h"
#include "data/stream_manager.h"  // stream_data_t
#include <deque>
#include <string>
#include <map>

// TCHAR compatibility
#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring tstring;
typedef wchar_t TCHAR;
#else
typedef std::string tstring;
typedef char TCHAR;
#endif

// Forward declarations
class ScriptStringManager;

/**
 * @brief Compiled DNF Lexer - Binary script token reader
 *
 * Reads tokens from binary compiled script files (.bcsf) with format:
 * - Magic: 0xD0B0
 * - Token stream: 1-byte token type + variable-length value
 * - String table: Indexed strings for efficient storage
 *
 * Supports nested #include directives via stream stack.
 */
class CompiledDNFLex {
public:
    // ========================================================================
    // Nested Types
    // ========================================================================

    /**
     * @brief Input stream descriptor for binary scripts
     */
    struct stream_t {
        std::string dir;              ///< Directory path
        std::string filename;         ///< File name
        char* buffer;                 ///< Binary buffer
        int size;                     ///< Buffer size

        stream_t();
        ~stream_t();
    };

    /**
     * @brief Callback: Error reporting
     */
    typedef void (*TLexerError)(const char* errorMsg);

    /**
     * @brief Callback: Load stream data
     */
    typedef bool (*TLoadStream)(const TCHAR* dir, const TCHAR* filename,
                               stream_data_t* streamData);

    // ========================================================================
    // Construction / Destruction
    // ========================================================================

    /**
     * @brief Default constructor (creates own string manager)
     * Source: df_game_r_part24.c:36041-36112
     */
    CompiledDNFLex();

    /**
     * @brief Constructor with external string manager
     * @param pStringManager External string manager (not owned)
     * Source: df_game_r_part24.c:35960-36039
     */
    explicit CompiledDNFLex(ScriptStringManager* pStringManager);

    /**
     * @brief Destructor
     * Source: df_game_r_part24.c:36386-36394
     */
    ~CompiledDNFLex();

    // ========================================================================
    // Token Reading
    // ========================================================================

    /**
     * @brief Get next token from stream
     * @return Token type
     *
     * Reads 1 byte from buffer as token type. Handles #include directives
     * by recursively loading and pushing new streams onto stack.
     *
     * Source: df_game_r_part24.c:36415-36542
     */
    DNFLEX_TOKEN getToken();

    /**
     * @brief Read integer value from current position
     * @param result Success flag (output, optional)
     * @return Integer value (4 bytes)
     * Source: df_game_r_part24.c:35665-35690
     */
    int getInt(bool* result);

    /**
     * @brief Read float value from current position
     * @param result Success flag (output, optional)
     * @return Float value (4 bytes)
     * Source: df_game_r_part24.c:35692-35715
     */
    float getFloat(bool* result);

    /**
     * @brief Read string from current position (into std::string)
     * @param stringBuffer Output string buffer
     * @return Success status
     *
     * Reads 2-byte string table index, looks up in string manager.
     * Source: df_game_r_part24.c:35767-35809
     */
    bool getStr(std::string* stringBuffer);

    /**
     * @brief Read string from current position (into char buffer)
     * @param buffer Output buffer
     * @param maxSize Buffer capacity
     * @return Success status
     * Source: df_game_r_part24.c:35811-35895
     */
    bool getStr(char* buffer, int maxSize);

    /**
     * @brief Get const char* pointer to last read string
     * @param result Success flag (output, optional)
     * @return String pointer (valid until next getStr call)
     *
     * Note: This method is used by DNFLexWrapper to cache string pointers.
     * Implementation returns pointer to internal stringBuffer_.
     */
    const char* getStr(bool* result);

    // ========================================================================
    // Stream Management
    // ========================================================================

    /**
     * @brief Switch to new input stream
     * @param dir Directory path
     * @param filename File name
     * @param buffer Binary buffer
     * @param size Buffer size
     * @return Success status
     *
     * Closes all existing streams and initializes new root stream.
     * Source: df_game_r_part24.c:36403-36413
     */
    bool SwitchInputStream(const TCHAR* dir, const TCHAR* filename,
                          char* buffer, int size);

    /**
     * @brief Push new stream onto stack (for #include)
     * @param dir Directory path
     * @param filename File name
     * @param buffer Binary buffer
     * @param bufferSize Buffer size
     * @return Success status
     * Source: df_game_r_part24.c:36120-36261
     */
    bool PushInputStream(const TCHAR* dir, const TCHAR* filename,
                        char* buffer, int bufferSize);

    /**
     * @brief Pop top stream from stack
     * @param pop_all If true, pop all streams; else keep at least 1
     * @return Success status (false if stack empty or only 1 remaining)
     * Source: df_game_r_part24.c:36263-36376
     */
    bool PopInputStream(bool pop_all);

    /**
     * @brief Close all input streams
     * Source: df_game_r_part24.c:36378-36384
     */
    void CloseInputStream();

    /**
     * @brief Get current stream descriptor
     * @return Pointer to top stream, or nullptr if empty
     */
    stream_t* GetInputStream();

    /**
     * @brief Get current read offset
     * @return Byte offset in current stream
     * Source: df_game_r_part24.c:35646-35663
     */
    int GetOffset() const;

    /**
     * @brief Check if stream is active
     * @return True if stream stack non-empty
     * Source: df_game_r_part24.c:35606-35644
     */
    bool IsStream() const;

    /**
     * @brief Check if reached end of stream
     * @return True if offset >= buffer size
     * Source: df_game_r_part24.c:35717-35730
     */
    bool isEnd();

    // ========================================================================
    // String Table Management
    // ========================================================================

    /**
     * @brief Get string manager instance
     * @return Pointer to string manager
     */
    ScriptStringManager* getStringManager();

    /**
     * @brief Set external string manager
     * @param stringManager External manager (ownership NOT transferred)
     * Source: df_game_r_part24.c:35732-35746
     */
    void setStringManager(ScriptStringManager* stringManager);

    // ========================================================================
    // Configuration
    // ========================================================================

    /**
     * @brief Set callback handlers
     * @param LexerErrorFunc Error callback
     * @param LoadStreamFunc Stream load callback
     * Source: df_game_r_part24.c:35571-35579
     */
    void SetHandler(TLexerError LexerErrorFunc, TLoadStream LoadStreamFunc);

    /**
     * @brief Disable #include directive processing
     * Source: df_game_r_part24.c:35748-35752
     */
    void DisableInclusion();

    /**
     * @brief Enable #include directive processing
     * Source: df_game_r_part24.c:35754-35758
     */
    void EnableInclusion();

    // ========================================================================
    // Internal Methods (public for testing, normally would be private)
    // ========================================================================

    /**
     * @brief Report error via callback
     * @param msg Error message
     * Source: df_game_r_part24.c:35581-35589
     */
    void LexerError(const char* msg);

    /**
     * @brief Load stream via callback
     * @param dir Directory
     * @param filename File name
     * @param streamData Output stream data
     * @return Success status
     * Source: df_game_r_part24.c:35591-35604
     */
    bool LoadStream(const TCHAR* dir, const TCHAR* filename,
                   stream_data_t* streamData);

protected:
    // ========================================================================
    // Internal Lifecycle
    // ========================================================================

    /**
     * @brief Initialize internal state
     * @param createStringTable If true, create owned string manager
     * Source: df_game_r_part24.c:35923-35958
     */
    void create(bool createStringTable);

    /**
     * @brief Destroy internal state
     * Source: df_game_r_part24.c:35897-35921
     */
    void destroy();

    // ========================================================================
    // Member Variables
    // ========================================================================

    /// Stream stack (std::deque for efficient push/pop at both ends)
    std::deque<stream_t> stream_stack_;

    /// Current read offset in active stream
    char* offset_;

    /// Last token read
    DNFLEX_TOKEN lastToken_;

    /// String manager (owned if stringManagerDelete_ is true)
    ScriptStringManager* stringManager_;

    /// Flag: true if we own stringManager_ and must delete it
    bool stringManagerDelete_;

    /// Internal string buffer for getStr() overloads
    std::string* stringBuffer_;

    /// Error message buffer
    char ERR_BUF[256];

    /// Flag: allow #include directive processing
    bool allow_inclusion_;

    /// Callback: error reporting
    TLexerError LexerError_;

    /// Callback: stream loading
    TLoadStream LoadStream_;
};

#endif // COMPILED_DNFLEX_H
