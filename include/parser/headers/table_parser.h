/**
 * @file table_parser.h
 * @brief Table parser for DNF game server script files
 *
 * This file contains the Arad_Script class, which provides CSV-like table parsing
 * functionality for game data files. Tables are organized with named columns and rows,
 * and data is stored in fixed 128-byte cells.
 *
 * Extracted from: df_game_r_part08.c (lines 36069-36733)
 * Original decompiled code from DNF game server
 */

#ifndef TABLE_PARSER_H
#define TABLE_PARSER_H

#include <string>
#include <map>
#include <cstddef>

/**
 * @class Arad_Script
 * @brief Table-based script parser with fixed-size cell storage
 *
 * Provides a 2D table structure with:
 * - Named columns and rows for easy lookup
 * - Fixed 128-byte cells for all data (stored as strings, converted on access)
 * - Dynamic growth: columns expand by +32, rows double
 * - Initial capacity: 32 columns × 256 rows
 *
 * ## File Format
 *
 * Tables are parsed from text files with this structure:
 * ```
 * [section_name]
 * col1  col2/alias  col3
 * row1  value11     value12  value13
 * row2  value21     value22  value23
 * ```
 *
 * - First non-whitespace line after `[section]` defines column names
 * - Column names can have aliases separated by `/` (e.g., `damage/dmg`)
 * - Each subsequent line defines one row (first cell is the row name)
 * - Cells are whitespace-delimited
 *
 * ## Usage Example
 *
 * ```cpp
 * Arad_Script table;
 * if (table.Load(fileData, "Skills")) {
 *     int damage = table.GetInt("fireball", "damage");
 *     float cooldown = table.GetFloat("fireball", "cooldown");
 *     std::string desc = table.GetString("fireball", "description");
 * }
 * ```
 */
class Arad_Script {
public:
    /**
     * @brief Default constructor
     * Initializes empty table with no allocated storage
     */
    Arad_Script();

    /**
     * @brief Constructor with file path
     * @param filePath Path to script file (stored but not loaded until Load() is called)
     */
    explicit Arad_Script(const char* filePath);

    /**
     * @brief Destructor - frees allocated cell storage
     */
    ~Arad_Script();

    // Prevent copying (original class is not copyable due to raw pointer ownership)
    Arad_Script(const Arad_Script&) = delete;
    Arad_Script& operator=(const Arad_Script&) = delete;

    /**
     * @brief Initialize table storage
     * @return true on success, false on allocation failure
     *
     * Allocates initial capacity: 32 columns × 256 rows × 128 bytes per cell
     * Called automatically by Load()
     */
    bool Create();

    /**
     * @brief Load table from in-memory data
     * @param data Pointer to text data (null-terminated)
     * @param sectionName Section name to find (e.g., "Skills" matches "[Skills]")
     * @return true on success, false if section not found or parse error
     *
     * Parses table data from memory buffer. Searches for `[sectionName]` header,
     * then parses column names and data rows.
     */
    bool Load(const char* data, const char* sectionName);

    /**
     * @brief Get column index by name
     * @param colName Column name to lookup
     * @return Column index (0-based), or -1 if not found
     */
    int GetIdx4Col(const char* colName) const;

    /**
     * @brief Get row index by name
     * @param rowName Row name to lookup
     * @return Row index (0-based), or -1 if not found
     */
    int GetIdx4Row(const char* rowName) const;

    // ===== GetString Methods (4 overloads) =====

    /**
     * @brief Get cell string value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @return Pointer to cell string (valid until table is modified)
     */
    const char* GetString(int row, int col) const;

    /**
     * @brief Get cell string value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @return Pointer to cell string, or empty string if not found
     */
    const char* GetString(const char* rowName, const char* colName) const;

    /**
     * @brief Get cell string value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @return Pointer to cell string, or empty string if not found
     */
    const char* GetString(const char* rowName, int col) const;

    /**
     * @brief Get cell string value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @return Pointer to cell string, or empty string if not found
     */
    const char* GetString(int row, const char* colName) const;

    // ===== GetInt Methods (4 overloads) =====

    /**
     * @brief Get cell integer value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @return Parsed integer value (uses atoi internally)
     */
    int GetInt(int row, int col) const;

    /**
     * @brief Get cell integer value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @return Parsed integer value
     */
    int GetInt(const char* rowName, const char* colName) const;

    /**
     * @brief Get cell integer value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @return Parsed integer value
     */
    int GetInt(const char* rowName, int col) const;

    /**
     * @brief Get cell integer value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @return Parsed integer value
     */
    int GetInt(int row, const char* colName) const;

    // ===== GetFloat Methods (4 overloads) =====

    /**
     * @brief Get cell float value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @return Parsed float value (uses atof internally, cast to float)
     */
    float GetFloat(int row, int col) const;

    /**
     * @brief Get cell float value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @return Parsed float value
     */
    float GetFloat(const char* rowName, const char* colName) const;

    /**
     * @brief Get cell float value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @return Parsed float value
     */
    float GetFloat(const char* rowName, int col) const;

    /**
     * @brief Get cell float value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @return Parsed float value
     */
    float GetFloat(int row, const char* colName) const;

    // ===== GetDouble Methods (4 overloads) =====

    /**
     * @brief Get cell double value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @return Parsed double value (uses atof internally)
     */
    double GetDouble(int row, int col) const;

    /**
     * @brief Get cell double value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @return Parsed double value
     */
    double GetDouble(const char* rowName, const char* colName) const;

    /**
     * @brief Get cell double value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @return Parsed double value
     */
    double GetDouble(const char* rowName, int col) const;

    /**
     * @brief Get cell double value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @return Parsed double value
     */
    double GetDouble(int row, const char* colName) const;

    // ===== SetString Methods (4 overloads) =====

    /**
     * @brief Set cell string value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @param value String value to set (max 127 bytes)
     */
    void SetString(int row, int col, const char* value);

    /**
     * @brief Set cell string value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @param value String value to set
     */
    void SetString(const char* rowName, const char* colName, const char* value);

    /**
     * @brief Set cell string value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @param value String value to set
     */
    void SetString(const char* rowName, int col, const char* value);

    /**
     * @brief Set cell string value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @param value String value to set
     */
    void SetString(int row, const char* colName, const char* value);

    // ===== SetInt Methods (4 overloads) =====

    /**
     * @brief Set cell integer value by row/column indices
     * @param row Row index (0-based)
     * @param col Column index (0-based)
     * @param value Integer value to set
     */
    void SetInt(int row, int col, int value);

    /**
     * @brief Set cell integer value by row/column names
     * @param rowName Row name
     * @param colName Column name
     * @param value Integer value to set
     */
    void SetInt(const char* rowName, const char* colName, int value);

    /**
     * @brief Set cell integer value by row name and column index
     * @param rowName Row name
     * @param col Column index (0-based)
     * @param value Integer value to set
     */
    void SetInt(const char* rowName, int col, int value);

    /**
     * @brief Set cell integer value by row index and column name
     * @param row Row index (0-based)
     * @param colName Column name
     * @param value Integer value to set
     */
    void SetInt(int row, const char* colName, int value);

    /**
     * @brief Get current number of columns
     * @return Number of defined columns (not capacity)
     */
    int GetColCount() const { return currentCols_; }

    /**
     * @brief Get current number of rows
     * @return Number of defined rows (not capacity)
     */
    int GetRowCount() const { return currentRows_; }

    /**
     * @brief Get column name by index
     * @param col Column index
     * @return Column name, or empty string if not found
     */
    std::string GetColName(int col) const;

    /**
     * @brief Get row name by index
     * @param row Row index
     * @return Row name, or empty string if not found
     */
    std::string GetRowName(int row) const;

private:
    /**
     * @brief Expand column capacity by 32
     * @return true on success, false on allocation failure
     */
    bool ExpandCol();

    /**
     * @brief Expand row capacity (double current size)
     * @return true on success, false on allocation failure
     */
    bool ExpandRow();

    /**
     * @brief Calculate cell pointer from row/column indices
     * @param row Row index
     * @param col Column index
     * @return Pointer to cell data (128 bytes)
     */
    char* GetCellPtr(int row, int col) const;

    // Cell storage
    static const int CELL_SIZE = 128;        ///< Fixed size per cell (bytes)
    static const int INITIAL_COLS = 32;      ///< Initial column capacity
    static const int INITIAL_ROWS = 256;     ///< Initial row capacity
    static const int COL_GROW_STEP = 32;     ///< Columns added per expansion

    int colCapacity_;       ///< Current column capacity
    int rowCapacity_;       ///< Current row capacity
    int currentCols_;       ///< Number of columns actually defined
    int currentRows_;       ///< Number of rows actually defined
    int dataSize_;          ///< Total allocated size (bytes)
    char* data_;            ///< Cell data storage (rowCapacity × colCapacity × CELL_SIZE bytes)

    // Name-to-index maps
    std::map<std::string, int> colNameToIndex_;  ///< Column name lookup
    std::map<std::string, int> rowNameToIndex_;  ///< Row name lookup

    // File path storage (for compatibility with original API)
    std::string filePath_;
    std::string sectionName_;
    std::string tempPath_;
};

#endif // TABLE_PARSER_H
