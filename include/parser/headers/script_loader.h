/**
 * @file script_loader.h
 * @brief Script loader infrastructure for DNF game server
 *
 * This file contains the Arad_ScriptLoader base class and script loading utilities.
 * The loader uses a Template Method pattern where open() handles file I/O and calls
 * the virtual parsing() method implemented by derived classes.
 *
 * Extracted from: df_game_r_part08.c (lines 45831-45834, 63843-63871)
 *                 df_game_r_part20.c (lines 78164-78220)
 * Original decompiled code from DNF game server
 */

#ifndef SCRIPT_LOADER_H
#define SCRIPT_LOADER_H

#include <string>
#include <vector>

// Forward declarations
typedef char TCHAR;

/**
 * @class Arad_ScriptLoader
 * @brief Base class for script file loaders using Template Method pattern
 *
 * This class provides infrastructure for loading and parsing script files:
 *
 * ## Design Pattern: Template Method
 *
 * - **open()** (non-virtual public method)
 *   1. Loads file using loadRDARScriptFile()
 *   2. Reads data from input stream
 *   3. Calls virtual parsing() method
 *   4. Cleans up resources
 *
 * - **parsing()** (virtual protected method)
 *   - Must be implemented by derived classes
 *   - Receives raw file data as char* buffer
 *   - Returns error code (0 = success)
 *
 * ## Usage Example
 *
 * ```cpp
 * class MyScriptLoader : public Arad_ScriptLoader {
 * protected:
 *     virtual int parsing(const char* data) override {
 *         // Parse data here
 *         return 0;  // Success
 *     }
 * };
 *
 * MyScriptLoader loader;
 * int result = loader.open("config.txt");
 * if (result == 0) {
 *     // Success
 * }
 * ```
 *
 * ## Error Codes
 *
 * - 0: Success
 * - 16: File not found or load error
 * - Other: Parser-specific error codes
 */
class Arad_ScriptLoader {
public:
    /**
     * @brief Constructor
     */
    Arad_ScriptLoader();

    /**
     * @brief Virtual destructor
     */
    virtual ~Arad_ScriptLoader();

    /**
     * @brief Load and parse script file (Template Method)
     * @param filename Script file name (relative to script directory)
     * @return Error code (0 = success, 16 = file not found, other = parse error)
     *
     * This method:
     * 1. Loads file using loadRDARScriptFile()
     * 2. Reads data into memory buffer
     * 3. Appends EOF marker (-1)
     * 4. Calls virtual parsing() method
     * 5. Cleans up buffer
     *
     * Thread safety: Not thread-safe (uses global g_dnfLex)
     */
    int open(const TCHAR* filename);

protected:
    /**
     * @brief Parse loaded script data (virtual method for derived classes)
     * @param data Pointer to file data (null-terminated, ends with -1 EOF marker)
     * @return Error code (0 = success, non-zero = parse error)
     *
     * Derived classes must implement this method to parse their specific format.
     * The data buffer is guaranteed to be:
     * - Valid until this method returns
     * - Null-terminated with additional -1 byte at end
     * - Owned by open() method (don't delete it)
     */
    virtual int parsing(const char* data) = 0;
};

// ===== File Loading Functions =====

/**
 * @brief Load script file into global DNFLex lexer
 * @param dir Base directory (can be nullptr for default)
 * @param filename Script file name
 * @return true on success, false on error
 *
 * This function:
 * 1. Optionally calls g_onLoadScriptFileFunction callback
 * 2. Uses DNFLexWrapper::switchInputStream() to load file
 * 3. Stores file data in global g_dnfLex instance
 *
 * Note: This function uses a global lexer instance, so it's not thread-safe
 */
bool loadRDARScriptFile(const TCHAR* dir, const TCHAR* filename);

/**
 * @brief Unload currently loaded script file
 *
 * Cleans up resources from the last loadRDARScriptFile() call
 */
void unloadRDARScriptFile();

/**
 * @struct STScriptFileList
 * @brief Script file list container
 *
 * Holds a list of script file paths loaded from a list file
 */
struct STScriptFileList {
    std::vector<std::string> files;

    /**
     * @brief Clear file list
     */
    void Clear() {
        files.clear();
    }

    /**
     * @brief Add file to list
     * @param filename File path to add
     */
    void Add(const std::string& filename) {
        files.push_back(filename);
    }

    /**
     * @brief Get number of files
     * @return File count
     */
    size_t GetCount() const {
        return files.size();
    }

    /**
     * @brief Get file path by index
     * @param index File index (0-based)
     * @return File path reference
     */
    const std::string& GetFile(size_t index) const {
        return files[index];
    }
};

/**
 * @brief Load script list file
 * @param list Output list to populate
 * @param dir Base directory
 * @param filename List file name
 * @return 0 on success, non-zero on error
 *
 * Parses a list file containing script file paths (one per line)
 * Format:
 * ```
 * file1.txt
 * subdir/file2.txt
 * file3.txt
 * ```
 */
int loadRDARScriptListFile(STScriptFileList* list, const TCHAR* dir, const TCHAR* filename);

/**
 * @brief Add scripts from list file to existing list
 * @param list Existing list to append to
 * @param dir Base directory
 * @param filename List file name
 * @return 0 on success, non-zero on error
 */
int addRDARScriptListFile(STScriptFileList* list, const TCHAR* dir, const TCHAR* filename);

// ===== Global Callback =====

/**
 * @brief Global callback function pointer for script loading events
 *
 * If set, this function is called before each script file is loaded.
 * Can be used for logging, progress tracking, or pre-processing.
 *
 * Example:
 * ```cpp
 * void onLoadScript() {
 *     printf("Loading script...\n");
 * }
 * g_onLoadScriptFileFunction = onLoadScript;
 * ```
 */
extern int (*g_onLoadScriptFileFunction)(void);

#endif // SCRIPT_LOADER_H
