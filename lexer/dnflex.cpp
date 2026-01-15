/**
 * @file dnflex.cpp
 * @brief DNF Script Lexical Analyzer - Implementation
 *
 * Original source: df_game_r_part24.c:33400-34186
 * Extraction date: 2025-10-23
 */

#include "lexer/dnflex.h"
#include <cstring>
#include <cstdio>

// External dependencies (from flex_runtime.cpp)
extern "C" {
    extern void* __dso_handle;
}

// ============================================================================
// Part 1: Simple Methods Implementation (简单方法实现，1-10行)
// ============================================================================

/**
 * @brief Get string value of last STRING token
 * Source: df_game_r_part24.c:33430-33433
 */
const char* DNFLex::tok_str()
{
    return this->STR_TOK_BUF;
}

/**
 * @brief Get length of last STRING token
 * Source: df_game_r_part24.c:33436-33439
 */
int DNFLex::tok_str_len()
{
    return this->m_str_tok_pos - this->STR_TOK_BUF;
}

/**
 * @brief Get current line number
 * Source: df_game_r_part24.c:33467-33470
 */
int DNFLex::getLineNumber()
{
    return this->yylineno;
}

/**
 * @brief Disable #include directive processing
 * Source: df_game_r_part24.c:33473-33476
 */
void DNFLex::DisableInclusion()
{
    this->allow_inclusion_ = false;
}

/**
 * @brief Enable #include directive processing
 * Source: df_game_r_part24.c:33479-33482
 */
void DNFLex::EnableInclusion()
{
    this->allow_inclusion_ = true;
}

/**
 * @brief Check if stream stack is not empty
 * Source: df_game_r_part24.c:33453-33456
 */
bool DNFLex::IsStream()
{
    return !this->stream_stack_.empty();
}

/**
 * @brief Get current input stream
 * Source: df_game_r_part24.c:33459-33464
 */
DNFLex::stream_t* DNFLex::GetInputStream()
{
    // Return NULL if stack is empty, otherwise return last element
    return (this->stream_stack_.empty()) ? nullptr : &this->stream_stack_.back();
}

/**
 * @brief Load stream via callback
 * Source: df_game_r_part24.c:33442-33450
 */
bool DNFLex::LoadStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s)
{
    TLoadStream LoadStream = this->LoadStream_;
    if (LoadStream) {
        return LoadStream(dir, filename, s);
    }
    return false;
}

/**
 * @brief Call LexerOutput callback
 * Source: df_game_r_part24.c:33410-33417
 */
void DNFLex::LexerOutput(const char* buf, int size)
{
    TLexerOutput LexerOutput = this->LexerOutput_;
    if (LexerOutput) {
        LexerOutput(buf, size);
    }
}

/**
 * @brief Call LexerError callback
 * Source: df_game_r_part24.c:33420-33427
 */
void DNFLex::LexerError(const char* msg)
{
    TLexerError LexerError = this->LexerError_;
    if (LexerError) {
        LexerError(msg);
    }
}

/**
 * @brief Close all input streams
 * Source: df_game_r_part24.c:33609-33613
 */
void DNFLex::CloseInputStream()
{
    // Pop all streams until stack is empty
    while (!this->stream_stack_.empty()) {
        this->PopInputStream(true); // pop_all = true
    }
}

// ============================================================================
// Part 2: Construction/Destruction and Configuration (构造/析构和配置)
// ============================================================================

/**
 * @brief Set callback functions
 * Source: df_game_r_part24.c:33400-33407
 */
void DNFLex::SetHandler(
    TIsDbcsLeadChar IsDbcsLeadCharFunc,
    TLexerOutput LexerOutputFunc,
    TLexerError LexerErrorFunc,
    TLoadStream LoadStreamFunc)
{
    this->IsDbcsLeadChar_ = IsDbcsLeadCharFunc;
    this->LexerOutput_ = LexerOutputFunc;
    this->LexerError_ = LexerErrorFunc;
    this->LoadStream_ = LoadStreamFunc;
}

/**
 * @brief Get current stream name (dir/filename)
 * Source: df_game_r_part24.c:33492-33507
 */
tstring* DNFLex::getStreamName(tstring* retstr)
{
    if (this->stream_stack_.empty()) {
        *retstr = "NONE";
    } else {
        stream_t& current = this->stream_stack_.back();
        *retstr = current.dir;
        retstr->append(current.filename);
    }
    return retstr;
}

/**
 * @brief Constructor
 * Source: df_game_r_part24.c:33512-33528
 */
DNFLex::DNFLex()
    : __dnf_script__FlexLexer(nullptr, nullptr) // Call base constructor
{
    // Clear error buffer
    memset(this->ERR_BUF, 0, sizeof(this->ERR_BUF));

    // Set virtual table pointer (in original: off_8E2EBA8)
    // Note: In C++, vtable is set automatically by compiler

    // Initialize stream stack to empty
    this->stream_stack_.clear();

    // Clear string token buffer
    this->m_str_tok_pos = nullptr;
    memset(this->STR_TOK_BUF, 0, sizeof(this->STR_TOK_BUF));

    // Set default flags
    this->is_dbcs_ = false;
    this->allow_inclusion_ = true; // Enable #include by default

    // Clear callback pointers
    this->IsDbcsLeadChar_ = nullptr;
    this->LexerOutput_ = nullptr;
    this->LexerError_ = nullptr;
    this->LoadStream_ = nullptr;
}

/**
 * @brief Destructor
 * Source: df_game_r_part24.c:33617-33684
 */
DNFLex::~DNFLex()
{
    // Set vtable (in original code, done manually)
    // In C++, vtable is managed automatically

    // Close all input streams
    this->CloseInputStream();

    // Clean up remaining stream_t objects in vector
    // (std::vector destructor handles this automatically)
    // In original code, manual cleanup of std::string members:
    for (auto& stream : this->stream_stack_) {
        // stream.dir and stream.filename are std::string, auto-destroyed
        // stream_data_t cleanup is done by stream_data_destroy()
    }

    // Clear vector (destructor will be called)
    this->stream_stack_.clear();

    // Base class destructor called automatically
}

/**
 * @brief Nested structure: stream_t destructor
 * Source: df_game_r_part24.c:33896-33934
 */
DNFLex::stream_t::~stream_t()
{
    // In original code, manual reference counting for std::string
    // In modern C++, std::string destructor handles this automatically
    // dir and filename are destroyed automatically
}

/**
 * @brief Destroy stream data resources
 * Source: df_game_r_part24.c:33532-33544
 */
void DNFLex::stream_data_destroy(stream_data_t* s)
{
    // Delete stream if exists
    if (s->stream) {
        // Call stream destructor via vtable (virtual destructor)
        delete s->stream;
        s->stream = nullptr;
    }

    // Delete buffer if exists
    if (s->buffer) {
        delete[] s->buffer;
        s->buffer = nullptr;
    }
}

// ============================================================================
// Part 3: Stream Management Methods (Stream 管理方法)
// ============================================================================

/**
 * @brief Switch to new input stream
 * Source: df_game_r_part24.c:33823-33831
 */
bool DNFLex::SwitchInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s)
{
    // Close all existing streams
    this->CloseInputStream();

    // Push new stream
    return this->PushInputStream(dir, filename, s);
}

/**
 * @brief Pop current stream from stack
 * Source: df_game_r_part24.c:33547-33606
 */
bool DNFLex::PopInputStream(bool pop_all)
{
    // Check if stack is empty
    if (this->stream_stack_.empty()) {
        return false;
    }

    // If pop_all is false, keep at least one stream
    if (!pop_all && this->stream_stack_.size() <= 1) {
        return false;
    }

    // Pop buffer state from Flex's internal stack
    this->yypop_buffer_state();

    // Get current stream (last element)
    stream_t& current = this->stream_stack_.back();

    // Destroy stream data
    DNFLex::stream_data_destroy(&current.s);

    // Remove from vector (this calls stream_t destructor for strings)
    this->stream_stack_.pop_back();

    // Restore line number from previous stream (or reset to 1)
    this->yy_start = 1; // Reset start condition

    if (!this->stream_stack_.empty()) {
        // Restore line number from previous stream
        this->yylineno = this->stream_stack_.back().line_number;
    } else {
        // No more streams, reset to line 1
        this->yylineno = 1;
    }

    return true;
}

/**
 * @brief Push new input stream onto stack
 * Source: df_game_r_part24.c:33695-33820
 */
bool DNFLex::PushInputStream(const TCHAR* dir, const TCHAR* filename, stream_data_t* s)
{
    // Create temporary stream_t structure
    stream_t stream;

    // Set directory (empty string if NULL)
    if (dir) {
        stream.dir = dir;
    } else {
        stream.dir = "";
    }

    // Set filename (empty string if NULL)
    if (filename) {
        stream.filename = filename;
    } else {
        stream.filename = "";
    }

    // Load stream data
    if (s) {
        // Use provided stream data
        stream.s = *s;
    } else {
        // Load via callback
        if (!this->LoadStream(dir, filename, &stream.s)) {
            // LoadStream failed
            sprintf(this->ERR_BUF, "%s - DNFLex::LoadStream failed.", "PushInputStream");
            this->LexerError(this->ERR_BUF);
            return false;
        }
    }

    // Save current line number to previous stream (if any)
    if (!this->stream_stack_.empty()) {
        this->stream_stack_.back().line_number = this->yylineno;
    }

    // Push stream onto stack
    this->stream_stack_.push_back(stream);

    // Create yy_buffer_state for new stream (via Flex vtable index 3)
    yy_buffer_state* buffer = this->yy_create_buffer(stream.s.stream, stream.s.size);

    // Push buffer state onto Flex's internal stack
    this->yypush_buffer_state(buffer);

    // Reset line number and start condition
    this->yy_start = 1;
    this->yylineno = 1;

    return true;
}

// ============================================================================
// Part 4: Core Token Extraction Method (核心令牌提取方法)
// ============================================================================

/**
 * @brief Get next token from input stream
 * Source: df_game_r_part24.c:33834-33893
 */
DNFLEX_TOKEN DNFLex::get_next_token()
{
    DNFLEX_TOKEN token = DNF_TOK_EOF;

    // Check if stream stack is empty
    if (this->stream_stack_.empty()) {
        return DNF_TOK_EOF;
    }

    // Call Flex lexer's yylex() via vtable (index 6)
    token = static_cast<DNFLEX_TOKEN>(this->yylex());

    // Handle #include directive (if enabled)
    if (this->allow_inclusion_ && token == DNF_TOK_INCLUDE) {
        // Get current stream info for error reporting
        stream_t* current = this->GetInputStream();

        // Next token should be STRING (filename to include)
        DNFLEX_TOKEN next_token = static_cast<DNFLEX_TOKEN>(this->yylex());

        if (next_token == DNF_TOK_STRING) {
            // Get filename from string token buffer
            const char* include_file = this->tok_str();

            // Try to push new stream
            if (this->PushInputStream("", include_file, nullptr)) {
                // Successfully loaded included file, get first token from it
                return this->get_next_token(); // Recursive call
            } else {
                // PushInputStream failed
                sprintf(this->ERR_BUF, "%s - %s/%s, line %d, PushInputStream failed.",
                        "get_next_token",
                        current ? current->dir.c_str() : "",
                        current ? current->filename.c_str() : "",
                        this->yylineno);
                this->LexerError(this->ERR_BUF);
                return DNF_TOK_ERROR;
            }
        } else {
            // #include not followed by STRING
            sprintf(this->ERR_BUF, "%s - %s/%s, line %d, invalid inclusion.",
                    "get_next_token",
                    current ? current->dir.c_str() : "",
                    current ? current->filename.c_str() : "",
                    this->yylineno);
            this->LexerError(this->ERR_BUF);
            return DNF_TOK_ERROR;
        }
    }

    // Handle EOF - pop stream and continue with previous stream
    if (token == DNF_TOK_EOF) {
        if (this->PopInputStream(false)) {
            // Successfully popped, get next token from previous stream
            return this->get_next_token(); // Recursive call
        }
    }

    return token;
}

// ============================================================================
// End of dnflex.cpp
// ============================================================================

