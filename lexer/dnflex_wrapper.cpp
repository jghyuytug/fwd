/**
 * @file dnflex_wrapper.cpp
 * @brief DNF Lexer Wrapper - Implementation
 *
 * Original source: df_game_r_part24.c:34188-34900
 * Extraction date: 2025-10-23
 */

#include "lexer/dnflex_wrapper.h"
#include "lexer/compiled_dnflex.h"
#include <cstdio>
#include <cstring>

// ============================================================================
// Part 1: Simple Configuration and Access Methods (简单配置和访问方法)
// ============================================================================

/**
 * @brief Set lexer type
 * Source: df_game_r_part24.c:34188-34191
 */
void DNFLexWrapper::setLexType(ENUM_LEX_TYPE lexType)
{
    this->lexType_ = lexType;
}

/**
 * @brief Get current lexer type
 * Source: df_game_r_part24.c:34194-34197
 */
DNFLexWrapper::ENUM_LEX_TYPE DNFLexWrapper::getLexType()
{
    return this->lexType_;
}

/**
 * @brief Load stream callback dispatcher
 * Source: df_game_r_part24.c:34200-34212
 */
bool DNFLexWrapper::LoadStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s)
{
    TLoadStream LoadStream = this->LoadStream_;
    if (LoadStream) {
        return LoadStream(dir, filename, s);
    }
    return false;
}

/**
 * @brief Get last token type
 * Source: df_game_r_part24.c:34320-34323
 */
DNFLEX_TOKEN DNFLexWrapper::getLastToken()
{
    return this->lastToken_;
}

/**
 * @brief Get last token data as raw pointer
 * Source: df_game_r_part24.c:34326-34338
 */
const char* DNFLexWrapper::getLastTokenData()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.yytext;  // Access Flex yytext
    } else if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    return nullptr;
}

/**
 * @brief Close all input streams
 * Source: df_game_r_part24.c:34638-34642
 */
void DNFLexWrapper::closeInputStream()
{
    this->dnfLex_.CloseInputStream();
    this->compiledDnfLex_.CloseInputStream();
}

// ============================================================================
// Part 2: Token Access Methods (Token 访问方法)
// ============================================================================

/**
 * @brief Get next token from current lexer
 * @return Token type
 *
 * This is the core token retrieval method. It dispatches to DNFLex or CompiledDNFLex
 * based on lexType_, and caches the token value in internal members for later access.
 *
 * For compiled mode (binary scripts):
 * - DECIMAL/HEXADECIMAL/PAIR_INT → caches to lastIntValue_
 * - FLOAT → caches to lastFloatValue_
 * - STRING/TYPE/CUSTOM_DATA/etc → caches to lastStringValue_
 *
 * Source: df_game_r_part24.c:34516-34551
 */
DNFLEX_TOKEN DNFLexWrapper::getToken()
{
    ENUM_LEX_TYPE lexType = this->lexType_;
    DNFLEX_TOKEN result;

    if (lexType == LEX_TYPE_NORMAL) {
        // Text script mode: delegate to DNFLex
        result = this->dnfLex_.get_next_token();
        this->lastToken_ = result;
        return result;
    }

    if (lexType != LEX_TYPE_COMPILED) {
        // Unknown mode: return cached token
        return this->lastToken_;
    }

    // Binary script mode: delegate to CompiledDNFLex
    result = this->compiledDnfLex_.getToken();
    this->lastToken_ = result;

    // Cache the token value based on its type
    if ((unsigned int)result > DNF_TOK_ERROR) {
        // Integer tokens: DECIMAL(2), HEXADECIMAL(3), PAIR_INT(8)
        if (((unsigned int)(result - 2) <= 1) || (result == DNF_TOK_PAIR_INT)) {
            this->lastIntValue_ = this->compiledDnfLex_.getInt(nullptr);
            return this->lastToken_;
        }

        // Float token: FLOAT(5)
        if (result == DNF_TOK_FLOAT) {
            this->lastFloatValue_ = this->compiledDnfLex_.getFloat(nullptr);
            return this->lastToken_;
        }

        // String-like tokens: STRING(7), TYPE(6), CUSTOM_DATA(10), PAIR_STR(9), etc.
        {
            bool success = false;
            this->lastStringValue_ = this->compiledDnfLex_.getStr(&success);
        }
    }

    return this->lastToken_;
}

/**
 * @brief Get decimal integer from current token
 * @return Integer value
 *
 * For text mode: Parses dnfLex_.yytext manually
 * - Skips leading whitespace
 * - Handles '+' and '-' signs
 * - Converts digit sequence to integer
 *
 * For binary mode: Returns cached lastIntValue_
 *
 * Source: df_game_r_part24.c:34215-34272
 */
int DNFLexWrapper::getDecimal()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastIntValue_;
    }

    if (lexType != LEX_TYPE_NORMAL) {
        return 0;
    }

    // Text mode: manual parsing from yytext
    char* yytext = this->dnfLex_.yytext;
    char ch = *yytext;

    if (!ch) {
        return 0;
    }

    // Skip leading whitespace and find sign
    bool is_negative = false;
    while (true) {
        // Check if digit
        if ((unsigned char)(ch - '0') <= 9) {
            // Start parsing number
            break;
        }

        // Skip spaces
        if (ch == ' ') {
            ch = *++yytext;
            if (!*yytext) {
                return 0;
            }
            continue;
        }

        // Handle sign
        if (ch == '+') {
            ++yytext;
            is_negative = false;
            ch = *yytext;
            break;
        }

        if (ch == '-') {
            ++yytext;
            is_negative = true;
            ch = *yytext;
            break;
        }

        // Invalid character
        return 0;
    }

    // Parse digit sequence
    int result = 0;
    for (unsigned int digit = ch - '0'; digit <= 9; digit = *yytext - '0') {
        ++yytext;
        result = digit + 10 * result;
    }

    if (is_negative) {
        return -result;
    }

    return result;
}

/**
 * @brief Get hexadecimal integer from current token
 * @return Integer value
 *
 * For text mode: Skips "0x" prefix and uses sscanf("%x")
 * For binary mode: Returns cached lastIntValue_
 *
 * Source: df_game_r_part24.c:34491-34513
 */
int DNFLexWrapper::getHexDecimal()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastIntValue_;
    }

    if (lexType != LEX_TYPE_NORMAL) {
        return 0;
    }

    // Text mode: parse from yytext (skip "0x" prefix)
    char* yytext = this->dnfLex_.yytext;
    int rval = 0;
    sscanf(yytext + 2, "%x", &rval);  // Skip "0x"
    return rval;
}

/**
 * @brief Get floating-point value from current token
 * @return Float value
 *
 * For text mode: Uses strtod() to parse yytext
 * For binary mode: Returns cached lastFloatValue_
 *
 * Source: df_game_r_part24.c:34472-34488
 */
float DNFLexWrapper::getFloat()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastFloatValue_;
    }

    if (lexType != LEX_TYPE_NORMAL) {
        return 0.0f;
    }

    // Text mode: parse from yytext
    return (float)strtod(this->dnfLex_.yytext, nullptr);
}

/**
 * @brief Get last line number or offset
 * @return Line number (text mode) or byte offset (binary mode)
 *
 * Source: df_game_r_part24.c:34348-34360
 */
int DNFLexWrapper::getLastLineNo()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.yylineno;
    }

    if (lexType == LEX_TYPE_COMPILED) {
        return this->compiledDnfLex_.GetOffset();
    }

    return -1;
}

/**
 * @brief Get last token data as std::string
 * @param retstr Output string (return by value via pointer)
 * @return Pointer to retstr
 *
 * Converts the last token data to a std::tstring.
 * For text mode: Uses dnfLex_.yytext
 * For binary mode: Uses lastStringValue_
 *
 * Source: df_game_r_part24.c:34363-34388
 */
tstring* DNFLexWrapper::getLastTokenDataAsString(tstring* retstr)
{
    ENUM_LEX_TYPE lexType = this->lexType_;
    const char* str_ptr = nullptr;

    if (lexType == LEX_TYPE_NORMAL) {
        str_ptr = this->dnfLex_.yytext;
    } else if (lexType == LEX_TYPE_COMPILED) {
        str_ptr = this->lastStringValue_;
    }

    if (str_ptr) {
        // Convert to tstring (assuming toTString exists)
        // Note: Original code references toTString() function
        *retstr = str_ptr;  // Simplified: direct assignment
    } else {
        *retstr = "";
    }

    return retstr;
}

// ============================================================================
// Part 3: String Access Methods (String 访问方法)
// ============================================================================

/**
 * @brief Get string token value
 * @return String pointer
 *
 * For text mode: Returns DNFLex::tok_str() (processed string buffer)
 * For binary mode: Returns cached lastStringValue_
 *
 * Source: df_game_r_part24.c:34457-34469
 */
const char* DNFLexWrapper::getString()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.tok_str();
    }

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    return nullptr;
}

/**
 * @brief Get type string
 * @return Type string pointer
 *
 * For text mode: Returns yytext directly
 * For binary mode: Returns cached lastStringValue_
 *
 * Source: df_game_r_part24.c:34275-34287
 */
const char* DNFLexWrapper::getType()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.yytext;
    }

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    return nullptr;
}

/**
 * @brief Get custom data string
 * @return Custom data pointer
 *
 * For text mode: Returns yytext directly
 * For binary mode: Returns cached lastStringValue_
 *
 * Source: df_game_r_part24.c:34290-34302
 */
const char* DNFLexWrapper::getCustomData()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.yytext;
    }

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    return nullptr;
}

/**
 * @brief Get connector string
 * @return Connector string pointer
 *
 * For text mode: Returns yytext directly
 * For binary mode: Returns cached lastStringValue_
 *
 * Source: df_game_r_part24.c:34305-34317
 */
const char* DNFLexWrapper::getConnector()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        return this->dnfLex_.yytext;
    }

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    return nullptr;
}

/**
 * @brief Get string part of pair token [int "string"]
 * @return String pointer (without quotes)
 *
 * For text mode: Extracts string from yytext, removes surrounding quotes
 * - Format: [123 "hello"] → "hello"
 * - Copies to static buffer, strips first and last character
 *
 * For binary mode: Returns cached lastStringValue_
 *
 * Source: df_game_r_part24.c:34410-34431
 */
const char* DNFLexWrapper::getPairStr()
{
    static char buffer[256];  // Static buffer for pair string
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastStringValue_;
    }

    if (lexType != LEX_TYPE_NORMAL) {
        return nullptr;
    }

    // Text mode: extract string from yytext
    // yytext format: [123 "hello"]
    // We need to extract: hello (without quotes)

    // Skip first character '[' and find the quoted string
    strncpy(buffer, this->dnfLex_.yytext + 1, 0x100);

    // Find the end and null-terminate (remove trailing ']')
    size_t len = strlen(buffer);
    if (len > 0) {
        buffer[len - 1] = '\0';  // Remove last character
    }

    return buffer;
}

/**
 * @brief Get integer part of pair token [int "string"]
 * @return Integer value
 *
 * For text mode: Extracts integer from yytext
 * - Format: [123 "hello"] → 123
 * - Copies to local buffer, removes brackets, parses with strtol
 *
 * For binary mode: Returns cached lastIntValue_
 *
 * Source: df_game_r_part24.c:34434-34454
 */
int DNFLexWrapper::getPairInt()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_COMPILED) {
        return this->lastIntValue_;
    }

    if (lexType != LEX_TYPE_NORMAL) {
        return 0;
    }

    // Text mode: extract integer from yytext
    // yytext format: [123 "hello"]
    // We need to extract: 123

    char buffer[264];
    strncpy(buffer, this->dnfLex_.yytext + 1, 0x100);  // Skip '['

    // Remove trailing ']'
    size_t len = strlen(buffer);
    if (len > 0) {
        buffer[len - 1] = '\0';
    }

    // Parse integer
    return (int)strtol(buffer, nullptr, 10);
}

// ============================================================================
// Part 4: Stream Management Methods (Stream 管理方法)
// ============================================================================

/**
 * @brief Get current input stream pointer (text mode only)
 * @return std::istream pointer or nullptr
 *
 * Binary mode does not expose std::istream
 *
 * Source: df_game_r_part24.c:34565-34573
 */
std::istream* DNFLexWrapper::getInputStream()
{
    if (this->lexType_ == LEX_TYPE_NORMAL) {
        DNFLex::stream_t* stream_info = this->dnfLex_.GetInputStream();
        if (stream_info) {
            return stream_info->s.stream;
        }
    }

    return nullptr;
}

/**
 * @brief Get current input stream size
 * @return Stream size in bytes, or -1 on error
 *
 * Source: df_game_r_part24.c:34576-34588
 */
int DNFLexWrapper::getInputStreamSize()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        DNFLex::stream_t* stream_info = this->dnfLex_.GetInputStream();
        if (stream_info) {
            return stream_info->s.size;
        }
    } else if (lexType == LEX_TYPE_COMPILED) {
        // CompiledDNFLex::GetInputStream() returns a structure with size field
        // For now, stub returns -1
        return -1;  // TODO: Implement in Phase 5
    }

    return -1;
}

/**
 * @brief Get current input stream buffer pointer
 * @return Buffer pointer or nullptr
 *
 * Source: df_game_r_part24.c:34591-34603
 */
char* DNFLexWrapper::getInputStreamBuffer()
{
    ENUM_LEX_TYPE lexType = this->lexType_;

    if (lexType == LEX_TYPE_NORMAL) {
        DNFLex::stream_t* stream_info = this->dnfLex_.GetInputStream();
        if (stream_info) {
            return stream_info->s.buffer;
        }
    } else if (lexType == LEX_TYPE_COMPILED) {
        // CompiledDNFLex::GetInputStream() returns a structure with buffer field
        return nullptr;  // TODO: Implement in Phase 5
    }

    return nullptr;
}

/**
 * @brief Get current stream name
 * @param retstr Output string (return by value via pointer)
 * @return Pointer to retstr
 *
 * Source: df_game_r_part24.c:34554-34562
 */
tstring* DNFLexWrapper::getStreamName(tstring* retstr)
{
    if (this->lexType_ == LEX_TYPE_NORMAL) {
        this->dnfLex_.getStreamName(retstr);
    } else {
        *retstr = "NONE";
    }

    return retstr;
}

/**
 * @brief Set string table from file (for compiled scripts)
 * @param fileName String table file path
 * @return Success status
 *
 * Delegates to CompiledDNFLex's ScriptStringManager
 *
 * Source: df_game_r_part24.c:34392-34398
 */
bool DNFLexWrapper::setStringTableFromFile(const TCHAR* fileName)
{
    // Stub: will be implemented in Phase 5
    // Original: ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    //           return mgr->importFromFile(fileName);
    return false;
}

/**
 * @brief Set string table from memory buffer (for compiled scripts)
 * @param buffer Buffer containing string table data
 * @param size Buffer size
 * @return Success status
 *
 * Source: df_game_r_part24.c:34401-34407
 */
bool DNFLexWrapper::setStringTableFromBuffer(const unsigned char* buffer, int size)
{
    // Stub: will be implemented in Phase 5
    // Original: ScriptStringManager* mgr = compiledDnfLex_.getStringManager();
    //           return mgr->importFromBuffer(buffer, size);
    return false;
}

/**
 * @brief Set stream data from memory buffer
 * @param s Output stream_data_t structure
 * @param buffer Memory buffer (takes ownership if bufferSize > 0)
 * @param bufferSize Buffer size
 * @return Always true
 *
 * Creates an istrstream from the buffer.
 * If bufferSize is 0, frees buffer and creates empty stream.
 *
 * Source: df_game_r_part24.c:34606-34635
 */
bool DNFLexWrapper::setStringStreamData(stream_data_t* s, char* buffer, unsigned int bufferSize)
{
    if (bufferSize > 0) {
        // Create istrstream from buffer
        // Note: Original uses std::istrstream (deprecated)
        // In real implementation, would use:
        // std::locale* stream_obj = new std::istrstream(buffer, bufferSize);
        // s->stream = (std::istream*)stream_obj;
        s->stream = nullptr;  // Simplified stub
        s->size = bufferSize;
        s->buffer = buffer;
    } else {
        // Empty buffer: free it and create minimal stream
        delete[] buffer;

        static char kEmptyBuf[1] = {0};
        // std::locale* stream_obj = new std::istrstream(kEmptyBuf, 1);
        // s->stream = (std::istream*)stream_obj;
        s->stream = nullptr;  // Simplified stub
        s->size = 1;
        s->buffer = nullptr;
    }

    return true;
}

/**
 * @brief Set stream data from file
 * @param s Output stream_data_t structure
 * @param path File path
 * @return Success status
 *
 * Opens file using std::ifstream, gets file size, creates stream object.
 * If file is empty or fails to open, delegates to setStringStreamData with empty buffer.
 *
 * Source: df_game_r_part24.c:34756-34828
 *
 * NOTE: Original implementation is ~70 lines of complex std::ifstream setup
 * involving vtable manipulation and low-level C++ runtime initialization.
 * This is a simplified version capturing the core logic.
 */
bool DNFLexWrapper::setFileStreamData(stream_data_t* s, const TCHAR* path)
{
    if (!path || !s) {
        return false;
    }

    // Clear existing stream data
    stream_data_t::clear(s);

    // Original implementation:
    // 1. Allocates std::ifstream object (new 0x118 bytes)
    // 2. Calls std::ios_base::ios_base() constructor
    // 3. Sets up vtable pointers (off_8CFE970, off_8D0264C, etc.)
    // 4. Calls std::filebuf::basic_filebuf() and std::filebuf::open()
    // 5. Checks if file is open and readable
    // 6. Uses seekg(0, end) + tellg() to get file size
    // 7. Resets to beginning with seekg(0)
    // 8. Returns ifstream pointer and size

    // Simplified stub (real implementation would use std::ifstream):
    /*
    std::ifstream* file_stream = new std::ifstream(path, std::ios::in | std::ios::binary);
    if (!file_stream->is_open() || file_stream->fail()) {
        delete file_stream;
        return false;
    }

    file_stream->seekg(0, std::ios::end);
    uint32 file_size = (uint32)file_stream->tellg();
    file_stream->seekg(0, std::ios::beg);

    if (file_size == 0) {
        delete file_stream;
        return setStringStreamData(s, nullptr, 0);
    }

    s->stream = file_stream;
    s->size = file_size;
    return true;
    */

    // For now, return stub
    return false;
}

/**
 * @brief Switch to new input stream (auto-detects text vs binary)
 * @param dir Directory path
 * @param filename File name
 * @param s Pre-loaded stream data (optional, nullptr = load from file)
 * @return Success status
 *
 * Core dispatcher method:
 * 1. Loads stream if not provided (via LoadStream callback)
 * 2. Reads first 2 bytes to check for magic number 0xD0B0
 * 3. If magic matches → LEX_TYPE_COMPILED, delegates to CompiledDNFLex
 * 4. Otherwise → LEX_TYPE_NORMAL, delegates to DNFLex
 *
 * Source: df_game_r_part24.c:34645-34707
 */
bool DNFLexWrapper::switchInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s)
{
    stream_data_t* p_s_created = s;
    stream_data_t s_created;

    // If stream not provided, load it via callback
    if (!s) {
        p_s_created = &s_created;
        stream_data_t::clear(&s_created);

        if (!this->LoadStream(dir, filename, &s_created)) {
            // Load failed: report error
            if (this->LexerError_) {
                char buf[1024];
                snprintf(buf, sizeof(buf), "%s/%s Read failed!",
                         dir ? dir : "", filename ? filename : "");
                this->LexerError_(buf);
            }
            return false;
        }
    }

    // Check for binary script magic number (0xD0B0)
    unsigned short magic = 0;
    if (p_s_created->size > 1) {
        // Read first 2 bytes
        p_s_created->stream->read((char*)&magic, 2);

        // Reset stream position
        p_s_created->stream->seekg(0, std::ios::beg);

        if (magic == 0xD0B0) {
            // Binary compiled script detected
            this->lexType_ = LEX_TYPE_COMPILED;

            if (p_s_created->buffer) {
                // Buffer available: use it directly
                bool result = this->compiledDnfLex_.SwitchInputStream(
                    dir, filename, p_s_created->buffer, p_s_created->size);

                // Clean up stream object
                if (p_s_created->stream) {
                    delete p_s_created->stream;
                }

                return result;
            } else {
                // No buffer: allocate and read from stream
                char* buffer = new char[p_s_created->size];
                p_s_created->stream->read(buffer, p_s_created->size);

                return this->compiledDnfLex_.SwitchInputStream(
                    dir, filename, buffer, p_s_created->size);
            }
        }
    }

    // Text script (no magic number)
    this->lexType_ = LEX_TYPE_NORMAL;
    return this->dnfLex_.SwitchInputStream(dir, filename, p_s_created);
}

// ============================================================================
// Part 5: Lifecycle Methods (生命周期方法)
// ============================================================================

/**
 * @brief Constructor - Initialize DNFLexWrapper
 *
 * Initializes both DNFLex and CompiledDNFLex sub-objects.
 * Sets default mode to LEX_TYPE_NORMAL (text scripts).
 * All callbacks are initially nullptr.
 *
 * Source: df_game_r_part24.c:34742-34752
 */
DNFLexWrapper::DNFLexWrapper()
    : dnfLex_()
    , compiledDnfLex_()
    , lexType_(LEX_TYPE_NORMAL)
    , lastToken_(DNF_TOK_EOF)
    , lastIntValue_(0)
    , lastFloatValue_(0.0f)
    , lastStringValue_(nullptr)
    , LexerError_(nullptr)
    , LoadStream_(nullptr)
{
    // Member initializer list handles all initialization
}

/**
 * @brief Destructor - Cleanup DNFLexWrapper
 *
 * Destroys sub-objects in reverse order (CompiledDNFLex, then DNFLex).
 * Original code also sets vtable pointer, but we omit that in clean C++.
 *
 * Source: df_game_r_part24.c:34725-34730
 */
DNFLexWrapper::~DNFLexWrapper()
{
    // Destructors will be called automatically in reverse order:
    // 1. compiledDnfLex_.~CompiledDNFLex()
    // 2. dnfLex_.~DNFLex()
}

/**
 * @brief Set callback handlers for both lexers
 * @param IsDbcsLeadCharFunc DBCS lead character detection (for DNFLex only)
 * @param LexerOutputFunc Output handler (for DNFLex only)
 * @param LexerErrorFunc Error reporting callback (shared)
 * @param LoadStreamFunc Stream loading callback (shared)
 *
 * Delegates to both sub-lexers and caches shared callbacks locally.
 *
 * Source: df_game_r_part24.c:34710-34721
 */
void DNFLexWrapper::SetHandler(
    TIsDbcsLeadChar IsDbcsLeadCharFunc,
    TLexerOutput LexerOutputFunc,
    TLexerError LexerErrorFunc,
    TLoadStream LoadStreamFunc)
{
    // Set handlers on DNFLex (all 4 callbacks)
    this->dnfLex_.SetHandler(
        IsDbcsLeadCharFunc,
        LexerOutputFunc,
        LexerErrorFunc,
        LoadStreamFunc);

    // Set handlers on CompiledDNFLex (only 2 callbacks)
    // Stub method for now - will be implemented in Phase 5
    // this->compiledDnfLex_.SetHandler(LexerErrorFunc, LoadStreamFunc);

    // Cache callbacks locally
    this->LexerError_ = LexerErrorFunc;
    this->LoadStream_ = LoadStreamFunc;
}

// ============================================================================
// End of dnflex_wrapper.cpp
// ============================================================================
