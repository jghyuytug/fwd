/**
 * @file script_loader.cpp
 * @brief Implementation of Arad_ScriptLoader and script loading utilities
 *
 * Extracted from df_game_r_part08.c (lines 63843-63871)
 *                 df_game_r_part20.c (lines 78164-78220)
 * Refactored from Hex-Rays decompiled code
 */

#include "parser/script_loader.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

// ===== External Dependencies (Stage 3 - Lexer Layer) =====

// Forward declarations of Stage 3 components
// These will be linked when integrating with full project

struct DNFLex {
    // Placeholder for lexer state
    // Real implementation in Stage 3
    void* inputStream;
    int streamSize;
};

namespace DNFLexWrapper {
    /**
     * @brief Switch input stream to new file
     * @param lex Lexer instance
     * @param dir Base directory
     * @param filename File name
     * @param flags Load flags
     * @return true on success, false on error
     */
    extern bool switchInputStream(DNFLex* lex, const char* dir, const char* filename, int flags);

    /**
     * @brief Get input stream size
     * @param lex Lexer instance
     * @return Stream size in bytes
     */
    extern int getInputStreamSize(DNFLex* lex);

    /**
     * @brief Get input stream pointer
     * @param lex Lexer instance
     * @return Stream pointer (std::istream*)
     */
    extern void* getInputStream(DNFLex* lex);
}

// Global lexer instance (defined in Stage 3)
extern DNFLex g_dnfLex;

// ===== Global Callback =====

int (*g_onLoadScriptFileFunction)(void) = NULL;

// ===== Global Script Directory =====

// Default script base directory (0x8BAC660 in original code)
static const char g_scriptBaseDirectory[] = "Script";

// ===== Arad_ScriptLoader Implementation =====

Arad_ScriptLoader::Arad_ScriptLoader() {
    // Virtual table pointer is set by compiler
}

Arad_ScriptLoader::~Arad_ScriptLoader() {
    // Virtual destructor for proper cleanup
}

int Arad_ScriptLoader::open(const TCHAR* filename) {
    // Step 1: Load script file into global lexer
    if (!loadRDARScriptFile(g_scriptBaseDirectory, filename)) {
        return 16;  // File not found error
    }

    // Step 2: Get input stream size
    int streamSize = DNFLexWrapper::getInputStreamSize(&g_dnfLex);

    // Step 3: Allocate buffer (size + 1 for EOF marker)
    char* buffer = new(std::nothrow) char[streamSize + 1];
    if (!buffer) {
        return 17;  // Memory allocation error
    }

    // Initialize buffer
    std::memset(buffer, 0, streamSize + 1);

    // Step 4: Read data from input stream
    std::istream* stream = static_cast<std::istream*>(
        DNFLexWrapper::getInputStream(&g_dnfLex)
    );

    if (stream) {
        stream->read(buffer, streamSize);
    }

    // Step 5: Append EOF marker (-1 as unsigned char = 255)
    buffer[streamSize] = static_cast<char>(255);

    // Step 6: Call virtual parsing() method
    int result = parsing(buffer);

    // Step 7: Cleanup
    delete[] buffer;

    return result;
}

// ===== File Loading Functions =====

bool loadRDARScriptFile(const TCHAR* dir, const TCHAR* filename) {
    // Validate filename
    if (!filename) {
        return false;
    }

    // Optional callback
    if (g_onLoadScriptFileFunction) {
        g_onLoadScriptFileFunction();
    }

    // Load file into global lexer
    // switchInputStream() will:
    // - Locate file in dir/filename
    // - Open file stream
    // - Store in g_dnfLex
    return DNFLexWrapper::switchInputStream(&g_dnfLex, dir, filename, 0);
}

void unloadRDARScriptFile() {
    // Cleanup current script file
    // In original code, this is handled implicitly by next switchInputStream() call
    // For explicit cleanup, you would close the stream in g_dnfLex
}

int loadRDARScriptListFile(STScriptFileList* list, const TCHAR* dir, const TCHAR* filename) {
    // Step 1: Load list file
    if (!loadRDARScriptFile(dir, filename)) {
        return 1;  // File not found
    }

    // Step 2: Clear existing list
    list->Clear();

    // Step 3: Get stream size
    int streamSize = DNFLexWrapper::getInputStreamSize(&g_dnfLex);

    // Step 4: Allocate buffer
    char* buffer = new(std::nothrow) char[streamSize + 1];
    if (!buffer) {
        return 2;  // Memory allocation error
    }

    std::memset(buffer, 0, streamSize + 1);

    // Step 5: Read data
    std::istream* stream = static_cast<std::istream*>(
        DNFLexWrapper::getInputStream(&g_dnfLex)
    );

    if (stream) {
        stream->read(buffer, streamSize);
    }

    // Step 6: Parse file list (one file per line)
    std::string currentFile;
    for (int i = 0; i < streamSize; ++i) {
        char ch = buffer[i];

        if (ch == '\n' || ch == '\r' || ch == 0 || ch == -1) {
            // End of line or end of file
            if (!currentFile.empty()) {
                list->Add(currentFile);
                currentFile.clear();
            }
        } else if (ch == ' ' || ch == '\t') {
            // Skip whitespace at beginning and end
            if (!currentFile.empty()) {
                currentFile += ch;
            }
        } else {
            currentFile += ch;
        }
    }

    // Add last file if exists
    if (!currentFile.empty()) {
        list->Add(currentFile);
    }

    // Step 7: Cleanup
    delete[] buffer;

    return 0;  // Success
}

int addRDARScriptListFile(STScriptFileList* list, const TCHAR* dir, const TCHAR* filename) {
    // Create temporary list
    STScriptFileList tempList;

    // Load into temporary list
    int result = loadRDARScriptListFile(&tempList, dir, filename);
    if (result != 0) {
        return result;
    }

    // Add all files from temporary list to main list
    for (size_t i = 0; i < tempList.GetCount(); ++i) {
        list->Add(tempList.GetFile(i));
    }

    return 0;  // Success
}
