/**
 * @file compiled_dnflex.cpp
 * @brief Compiled DNF Lexer - Implementation
 *
 * Original source: df_game_r_part24.c:35571-36705
 * Extraction date: 2025-10-24
 */

#include "lexer/compiled_dnflex.h"
#include "lexer/dnflex.h"
#include <cstring>
#include <cstdio>

// Forward declaration for ScriptStringManager (will be extracted separately)
// For now, we create a minimal stub
class ScriptStringManager {
public:
    ScriptStringManager() {}
    ~ScriptStringManager() {}

    bool getString(int index, std::string* output) {
        // Stub: Phase 5 will implement full string table
        return false;
    }

    bool importFromFile(const char* filename) { return false; }
    bool importFromBuffer(const unsigned char* buffer, int size) { return false; }
};

// ============================================================================
// Part 1: Nested Types and Basic Methods
// ============================================================================

/**
 * @brief stream_t constructor
 * Source: Implicit (deque element)
 */
CompiledDNFLex::stream_t::stream_t()
    : dir()
    , filename()
    , buffer(nullptr)
    , size(0)
{
}

/**
 * @brief stream_t destructor
 * Source: df_game_r_part24.c:36576-36614
 */
CompiledDNFLex::stream_t::~stream_t()
{
    // Strings are automatically destroyed
    // Buffer is NOT deleted here (ownership managed by caller)
}

// ============================================================================
// Configuration and Callback Methods
// ============================================================================

/**
 * @brief Set callback handlers
 * Source: df_game_r_part24.c:35571-35578
 */
void CompiledDNFLex::SetHandler(TLexerError LexerErrorFunc, TLoadStream LoadStreamFunc)
{
    this->LexerError_ = LexerErrorFunc;
    this->LoadStream_ = LoadStreamFunc;
}

/**
 * @brief Report error via callback
 * Source: df_game_r_part24.c:35581-35588
 */
void CompiledDNFLex::LexerError(const char* msg)
{
    TLexerError LexerError = this->LexerError_;
    if (LexerError) {
        LexerError(msg);
    }
}

/**
 * @brief Load stream via callback
 * Source: df_game_r_part24.c:35591-35603
 */
bool CompiledDNFLex::LoadStream(const TCHAR* dir, const TCHAR* filename,
                                stream_data_t* s)
{
    TLoadStream LoadStream = this->LoadStream_;
    if (LoadStream) {
        return LoadStream(dir, filename, s);
    }
    return false;
}

/**
 * @brief Disable #include directive processing
 * Source: df_game_r_part24.c:35748-35751
 */
void CompiledDNFLex::DisableInclusion()
{
    this->allow_inclusion_ = false;
}

/**
 * @brief Enable #include directive processing
 * Source: df_game_r_part24.c:35754-35757
 */
void CompiledDNFLex::EnableInclusion()
{
    this->allow_inclusion_ = true;
}

// ============================================================================
// String Manager Methods
// ============================================================================

/**
 * @brief Get string manager instance
 * Source: df_game_r_part24.c:35742-35745
 */
ScriptStringManager* CompiledDNFLex::getStringManager()
{
    return this->stringManager_;
}

/**
 * @brief Set external string manager
 * Source: df_game_r_part24.c:35732-35739
 *
 * Note: Original has 3-parameter version with allowOwner flag,
 * but header declares 1-parameter version. Implementation uses
 * stringManagerDelete_ = false (not owned).
 */
void CompiledDNFLex::setStringManager(ScriptStringManager* stringManager)
{
    this->stringManagerDelete_ = false;  // Not owned
    this->stringManager_ = stringManager;
}

// ============================================================================
// Stream Status Methods
// ============================================================================

/**
 * @brief Check if stream is active
 * Source: df_game_r_part24.c:35606-35609
 */
bool CompiledDNFLex::IsStream() const
{
    return !this->stream_stack_.empty();
}

/**
 * @brief Get current stream descriptor
 * Source: df_game_r_part24.c:35612-35626
 */
CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream()
{
    if (this->stream_stack_.empty()) {
        return nullptr;
    }
    return &this->stream_stack_.back();
}

/**
 * @brief Get current read offset
 * Source: df_game_r_part24.c:35646-35662
 */
int CompiledDNFLex::GetOffset() const
{
    if (this->stream_stack_.empty()) {
        return -1;
    }

    const stream_t& stream = this->stream_stack_.back();
    if (stream.buffer) {
        return this->offset_ - stream.buffer;
    }

    return -1;
}

/**
 * @brief Check if reached end of stream
 * Source: df_game_r_part24.c:35717-35729
 */
bool CompiledDNFLex::isEnd()
{
    if (this->stream_stack_.empty()) {
        return false;
    }

    const stream_t& stream = this->stream_stack_.back();
    return (stream.buffer != nullptr) &&
           (this->offset_ >= stream.buffer + stream.size);
}

// ============================================================================
// Token Value Reading Methods
// ============================================================================

/**
 * @brief Read integer value from current position
 * Source: df_game_r_part24.c:35665-35688
 */
int CompiledDNFLex::getInt(bool* result)
{
    DNFLEX_TOKEN lastToken = this->lastToken_;

    // Check if last token is integer type
    if (((unsigned int)(lastToken - 2) <= 1) ||  // DECIMAL(2) or HEXADECIMAL(3)
        (lastToken == DNF_TOK_PAIR_INT))         // PAIR_INT(8)
    {
        if (result) {
            *result = true;
        }

        // Read 4 bytes as int32
        int value = *reinterpret_cast<int*>(this->offset_);
        this->offset_ += 4;
        return value;
    }

    // Not an integer token
    if (result) {
        *result = false;
    }
    return 0;
}

/**
 * @brief Read float value from current position
 * Source: df_game_r_part24.c:35692-35713
 */
float CompiledDNFLex::getFloat(bool* result)
{
    if (this->lastToken_ == DNF_TOK_FLOAT) {
        if (result) {
            *result = true;
        }

        // Read 4 bytes as float
        float value = *reinterpret_cast<float*>(this->offset_);
        this->offset_ += 4;
        return value;
    }

    // Not a float token
    if (result) {
        *result = false;
    }
    return 0.0f;
}

/**
 * @brief Read string from current position (into std::string)
 * Source: df_game_r_part24.c:35767-35809
 *
 * Reads 2-byte string table index, looks up in string manager.
 */
bool CompiledDNFLex::getStr(std::string* stringBuffer)
{
    DNFLEX_TOKEN lastToken = this->lastToken_;

    // Check if token is string type
    if (lastToken == DNF_TOK_STRING ||
        lastToken == DNF_TOK_TYPE ||
        lastToken == DNF_TOK_CUSTOM_DATA ||
        lastToken == DNF_TOK_CONNECTOR ||
        lastToken == DNF_TOK_PAIR_STR)
    {
        // Read 2-byte string table index
        unsigned short stringIndex = *reinterpret_cast<unsigned short*>(this->offset_);
        this->offset_ += 2;

        // Look up in string manager
        if (this->stringManager_) {
            if (this->stringManager_->getString(stringIndex, stringBuffer)) {
                return true;
            }
        }

        // Fallback: empty string
        *stringBuffer = "";
        return false;
    }

    // Not a string token
    return false;
}

/**
 * @brief Read string from current position (into char buffer)
 * Source: df_game_r_part24.c:35811-35895
 */
bool CompiledDNFLex::getStr(char* buffer, int maxSize)
{
    std::string tempString;
    if (this->getStr(&tempString)) {
        strncpy(buffer, tempString.c_str(), maxSize - 1);
        buffer[maxSize - 1] = '\0';
        return true;
    }

    buffer[0] = '\0';
    return false;
}

/**
 * @brief Get const char* pointer to last read string
 * @param result Success flag (output, optional)
 * @return String pointer (valid until next getStr call)
 */
const char* CompiledDNFLex::getStr(bool* result)
{
    if (!this->stringBuffer_) {
        this->stringBuffer_ = new std::string();
    }

    if (this->getStr(this->stringBuffer_)) {
        if (result) *result = true;
        return this->stringBuffer_->c_str();
    }

    if (result) *result = false;
    return nullptr;
}

// ============================================================================
// Part 2: Lifecycle and Stream Management
// ============================================================================

/**
 * @brief Initialize internal state
 * Source: df_game_r_part24.c:35923-35939
 */
void CompiledDNFLex::create(bool createStringTable)
{
    if (createStringTable) {
        if (!this->stringManager_) {
            // Create owned string manager
            this->stringManager_ = new ScriptStringManager();
        }
        this->stringManagerDelete_ = true;
    }

    if (!this->stringBuffer_) {
        // Allocate internal string buffer (1024 bytes)
        this->stringBuffer_ = new std::string();
        this->stringBuffer_->reserve(1024);
    }
}

/**
 * @brief Destroy internal state
 * Source: df_game_r_part24.c:35897-35920
 */
void CompiledDNFLex::destroy()
{
    // Delete string manager if owned
    if (this->stringManagerDelete_) {
        if (this->stringManager_) {
            delete this->stringManager_;
            this->stringManager_ = nullptr;
        }
        this->stringManagerDelete_ = false;
    } else {
        // Not owned, just clear pointer
        this->stringManager_ = nullptr;
    }

    // Delete string buffer
    if (this->stringBuffer_) {
        delete this->stringBuffer_;
        this->stringBuffer_ = nullptr;
    }
}

/**
 * @brief Default constructor (creates own string manager)
 * Source: df_game_r_part24.c:36041-36112
 */
CompiledDNFLex::CompiledDNFLex()
    : stream_stack_()
    , offset_(nullptr)
    , lastToken_(DNF_TOK_EOF)
    , stringManager_(nullptr)
    , stringManagerDelete_(false)
    , stringBuffer_(nullptr)
    , allow_inclusion_(true)
    , LexerError_(nullptr)
    , LoadStream_(nullptr)
{
    memset(this->ERR_BUF, 0, sizeof(this->ERR_BUF));
    this->create(true);  // Create owned string manager
}

/**
 * @brief Constructor with external string manager
 * Source: df_game_r_part24.c:35960-36039
 */
CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : stream_stack_()
    , offset_(nullptr)
    , lastToken_(DNF_TOK_EOF)
    , stringManager_(pStringManager)
    , stringManagerDelete_(false)
    , stringBuffer_(nullptr)
    , allow_inclusion_(true)
    , LexerError_(nullptr)
    , LoadStream_(nullptr)
{
    memset(this->ERR_BUF, 0, sizeof(this->ERR_BUF));
    this->create(false);  // Don't create string manager (use external)
}

/**
 * @brief Destructor
 * Source: df_game_r_part24.c:36386-36394
 */
CompiledDNFLex::~CompiledDNFLex()
{
    this->CloseInputStream();
    this->destroy();
}

/**
 * @brief Close all input streams
 * Source: df_game_r_part24.c:36378-36382
 */
void CompiledDNFLex::CloseInputStream()
{
    while (!this->stream_stack_.empty()) {
        this->PopInputStream(true);
    }
}

/**
 * @brief Pop top stream from stack
 * Source: df_game_r_part24.c:36263-36375
 *
 * @param pop_all If true, pop all streams; else keep at least 1
 * @return Success status (false if stack empty or only 1 remaining)
 */
bool CompiledDNFLex::PopInputStream(bool pop_all)
{
    // Check if stack is empty
    if (this->stream_stack_.empty()) {
        return false;
    }

    // If not pop_all, require at least 2 elements (keep 1)
    if (!pop_all && this->stream_stack_.size() <= 1) {
        return false;
    }

    // Get top stream and delete its buffer
    stream_t& top = this->stream_stack_.back();
    if (top.buffer) {
        delete[] top.buffer;
        top.buffer = nullptr;
    }

    // Remove from stack
    this->stream_stack_.pop_back();

    // Update offset to previous stream (if exists)
    if (!this->stream_stack_.empty()) {
        // Note: Original code references stream_t::offset member,
        // but our struct doesn't have it. We'll keep offset_ as-is.
        // this->offset_ = this->stream_stack_.back().offset;
    }

    return true;
}

/**
 * @brief Push new stream onto stack (for #include)
 * Source: df_game_r_part24.c:36120-36261
 *
 * Note: Original implementation is ~140 lines of complex deque manipulation.
 * Simplified to use std::deque::push_back.
 */
bool CompiledDNFLex::PushInputStream(const TCHAR* dir, const TCHAR* filename,
                                     char* buffer, int bufferSize)
{
    // Create new stream descriptor
    stream_t stream;
    stream.dir = dir ? dir : "";
    stream.filename = filename ? filename : "";
    stream.buffer = buffer;
    stream.size = bufferSize;

    // Push onto stack
    this->stream_stack_.push_back(stream);

    // Set offset to beginning of new buffer
    this->offset_ = buffer;

    return true;
}

/**
 * @brief Switch to new input stream
 * Source: df_game_r_part24.c:36403-36413
 */
bool CompiledDNFLex::SwitchInputStream(const TCHAR* dir, const TCHAR* filename,
                                      char* buffer, int size)
{
    // Close all existing streams
    this->CloseInputStream();

    // Push new root stream
    return this->PushInputStream(dir, filename, buffer, size);
}

// ============================================================================
// Part 3: Core Token Reading (getToken with #include support)
// ============================================================================

/**
 * @brief Get next token from stream
 * Source: df_game_r_part24.c:36415-36542
 *
 * Reads 1 byte from buffer as token type. Handles #include directives
 * by recursively loading and pushing new streams onto stack.
 */
DNFLEX_TOKEN CompiledDNFLex::getToken()
{
    // Check if stream stack is empty
    if (this->stream_stack_.empty()) {
        this->lastToken_ = DNF_TOK_EOF;
        return DNF_TOK_EOF;
    }

    // Get current stream
    stream_t& stream = this->stream_stack_.back();

    // Check if reached end of current stream
    if (this->offset_ >= stream.buffer + stream.size) {
        this->lastToken_ = DNF_TOK_EOF;

        // Try to pop stream and continue
        if (this->PopInputStream(false)) {
            // Recursively get token from previous stream
            return this->getToken();
        }

        return this->lastToken_;
    }

    // Read 1-byte token type
    DNFLEX_TOKEN token = static_cast<DNFLEX_TOKEN>(*this->offset_);
    this->offset_++;
    this->lastToken_ = token;

    // Handle #include directive
    if (this->allow_inclusion_ && token == DNF_TOK_INCLUDE) {
        // Get current stream info for error messages
        const std::string* currentDir = nullptr;
        const std::string* currentFilename = nullptr;
        if (!this->stream_stack_.empty()) {
            currentDir = &this->stream_stack_.back().dir;
            currentFilename = &this->stream_stack_.back().filename;
        }

        // Read next token (should be STRING)
        token = static_cast<DNFLEX_TOKEN>(*this->offset_);
        this->offset_++;
        this->lastToken_ = token;

        if (token != DNF_TOK_STRING) {
            // Error: #include must be followed by string
            snprintf(this->ERR_BUF, sizeof(this->ERR_BUF),
                    "%s - %s/%s, invalid inclusion.",
                    "getToken",
                    currentDir ? currentDir->c_str() : "",
                    currentFilename ? currentFilename->c_str() : "");
            this->LexerError(this->ERR_BUF);
            return DNF_TOK_ERROR;
        }

        // Read include file path
        std::string includePath;
        if (!this->getStr(&includePath)) {
            // Error: failed to read string
            snprintf(this->ERR_BUF, sizeof(this->ERR_BUF),
                    "%s - %s/%s, invalid inclusion.",
                    "getToken",
                    currentDir ? currentDir->c_str() : "",
                    currentFilename ? currentFilename->c_str() : "");
            this->LexerError(this->ERR_BUF);
            return DNF_TOK_ERROR;
        }

        // Load the include file
        stream_data_t streamData;
        if (!this->LoadStream("", includePath.c_str(), &streamData)) {
            // Error: failed to load stream
            snprintf(this->ERR_BUF, sizeof(this->ERR_BUF),
                    "%s - %s%s, LoadStream failed.",
                    "getToken",
                    currentDir ? currentDir->c_str() : "",
                    includePath.c_str());
            this->LexerError(this->ERR_BUF);
            return DNF_TOK_ERROR;
        }

        // Prepare buffer for binary stream
        char* buffer = streamData.buffer;
        int bufferSize = streamData.size;

        if (!buffer) {
            // No buffer provided, need to read from stream
            buffer = new char[bufferSize];
            streamData.stream->read(buffer, bufferSize);
        }

        // Push the include file onto stream stack
        if (!this->PushInputStream("", includePath.c_str(), buffer, bufferSize)) {
            // Error: failed to push stream
            snprintf(this->ERR_BUF, sizeof(this->ERR_BUF),
                    "%s - %s%s, PushInputStream failed.",
                    "getToken",
                    currentDir ? currentDir->c_str() : "",
                    includePath.c_str());
            this->LexerError(this->ERR_BUF);
            return DNF_TOK_ERROR;
        }

        // Clean up stream object if exists
        if (streamData.stream) {
            delete streamData.stream;
        }

        // Recursively get token from included file
        return this->getToken();
    }

    // Normal token (not #include)
    return token;
}

// ============================================================================
// End of compiled_dnflex.cpp
// ============================================================================

