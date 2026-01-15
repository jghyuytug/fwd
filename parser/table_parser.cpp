/**
 * @file table_parser.cpp
 * @brief Implementation of Arad_Script table parser
 *
 * Extracted from df_game_r_part08.c (lines 36069-36733)
 * Refactored from Hex-Rays decompiled code
 */

#include "parser/table_parser.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

// ===== Constructor / Destructor =====

Arad_Script::Arad_Script()
    : colCapacity_(0)
    , rowCapacity_(0)
    , currentCols_(0)
    , currentRows_(0)
    , dataSize_(0)
    , data_(NULL)
{
}

Arad_Script::Arad_Script(const char* filePath)
    : colCapacity_(0)
    , rowCapacity_(0)
    , currentCols_(0)
    , currentRows_(0)
    , dataSize_(0)
    , data_(NULL)
    , filePath_(filePath)
{
}

Arad_Script::~Arad_Script() {
    if (data_) {
        delete[] data_;
        data_ = NULL;
    }
}

// ===== Storage Management =====

bool Arad_Script::Create() {
    colCapacity_ = INITIAL_COLS;
    rowCapacity_ = INITIAL_ROWS;
    dataSize_ = (colCapacity_ * rowCapacity_) << 7;  // * 128 bytes per cell

    data_ = new(std::nothrow) char[dataSize_];
    if (!data_) {
        return false;
    }

    std::memset(data_, 0, dataSize_);
    currentCols_ = 0;
    currentRows_ = 0;
    colNameToIndex_.clear();
    rowNameToIndex_.clear();

    return true;
}

bool Arad_Script::ExpandCol() {
    colCapacity_ += COL_GROW_STEP;
    int newDataSize = (rowCapacity_ * colCapacity_) << 7;

    char* newData = new(std::nothrow) char[newDataSize];
    if (!newData) {
        return false;
    }

    std::memset(newData, 0, newDataSize);

    // Copy existing data
    // Original uses simple realloc semantics, but we need to preserve layout
    // NOTE: The original code just deletes old data without copying!
    // This appears to be a bug in the original, but we preserve the behavior
    if (data_) {
        delete[] data_;
    }

    data_ = newData;
    dataSize_ = newDataSize;

    return true;
}

bool Arad_Script::ExpandRow() {
    rowCapacity_ *= 2;
    int newDataSize = (rowCapacity_ * colCapacity_) << 7;

    char* newData = new(std::nothrow) char[newDataSize];
    if (!newData) {
        return false;
    }

    // Copy existing data
    std::memcpy(newData, data_, dataSize_);
    // Zero out new area (from old dataSize to newDataSize)
    std::memset(newData + dataSize_, 0, newDataSize - dataSize_);

    if (data_) {
        delete[] data_;
    }

    data_ = newData;
    dataSize_ = newDataSize;

    return true;
}

// ===== Cell Access Helper =====

char* Arad_Script::GetCellPtr(int row, int col) const {
    // Cell address = data + (row * colCapacity + col) * CELL_SIZE
    return data_ + ((row * colCapacity_ + col) << 7);
}

// ===== Table Loading =====

bool Arad_Script::Load(const char* data, const char* sectionName) {
    if (!Create()) {
        return false;
    }

    // Build section header to search for: "[sectionName]"
    char sectionHeader[1024];
    std::snprintf(sectionHeader, sizeof(sectionHeader), "[%s]", sectionName);
    size_t sectionHeaderLen = std::strlen(sectionHeader);

    // Search for section header
    const char* ptr = data;
    int matchPos = 0;
    int ch;

    // Skip to section header
    while (true) {
        ch = static_cast<unsigned char>(*ptr++);
        if (ch == 0 || ch == 255) {  // EOF equivalent (-1 cast to unsigned char is 255)
            return false;
        }

        // Skip non-printable characters
        if (ch <= 32 || ch > 126) {
            continue;
        }

        // Match section header
        if (sectionHeader[matchPos] == ch) {
            matchPos++;
            if (matchPos == static_cast<int>(sectionHeaderLen)) {
                break;  // Found section header
            }
        } else {
            matchPos = 0;
        }
    }

    // Parse column headers
    char token[512];
    int tokenLen = 0;

    // Read tokens until newline
    while (true) {
        ch = static_cast<unsigned char>(*ptr++);
        if (ch == 0 || ch == 255) {
            return false;
        }

        // Whitespace terminates token
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if (tokenLen > 0) {
                token[tokenLen] = '\0';
                tokenLen = 0;

                // Process column name(s) - may have aliases separated by '/'
                char* namePtr = token;
                size_t nameLen = std::strlen(token);

                for (size_t i = 0; i < nameLen; ++i) {
                    if (token[i] == '/') {
                        token[i] = '\0';
                        colNameToIndex_[namePtr] = currentCols_;
                        namePtr = &token[i + 1];
                    }
                }

                // Add final name (or only name if no aliases)
                colNameToIndex_[namePtr] = currentCols_;
                currentCols_++;

                // Expand columns if needed
                if (currentCols_ + 1 >= colCapacity_ && !ExpandCol()) {
                    return false;
                }
            }

            if (ch == '\n') {
                break;  // End of column header line
            }
        } else {
            token[tokenLen++] = ch;
        }
    }

    // Parse data rows
    while (true) {
        int colIdx = -1;  // -1 means we're reading the row name
        tokenLen = 0;

        while (true) {
            ch = static_cast<unsigned char>(*ptr++);

            // Whitespace or EOF terminates token
            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == 0 || ch == 255) {
                if (tokenLen > 0) {
                    token[tokenLen] = '\0';
                    tokenLen = 0;

                    if (colIdx == -1) {
                        // First token is row name
                        rowNameToIndex_[token] = currentRows_;
                    } else {
                        // Data cell
                        char* cellPtr = GetCellPtr(currentRows_, colIdx);
                        std::strncpy(cellPtr, token, CELL_SIZE - 1);
                        cellPtr[CELL_SIZE - 1] = '\0';
                    }

                    colIdx++;

                    // If we've filled all columns, skip rest of line
                    if (colIdx >= currentCols_) {
                        while (ch != 0 && ch != 255 && ch != '\n') {
                            ch = static_cast<unsigned char>(*ptr++);
                        }
                    }
                } else {
                    // Empty token - possible at line end
                    if (ch == '\n' || ch == 0 || ch == 255) {
                        ch = 255;  // Force break
                    }
                }

                if (ch == '\n' || ch == 0 || ch == 255) {
                    break;  // End of line
                }
            } else {
                token[tokenLen++] = ch;
            }
        }

        // Validate row had correct number of columns
        if (colIdx != currentCols_ && colIdx != 0) {
            // colIdx == 0 means empty line (only whitespace)
            if (colIdx > 0) {
                return false;  // Mismatched column count
            }
        }

        // Only count as a row if we had data
        if (colIdx > 0) {
            currentRows_++;

            // Expand rows if needed
            if (currentRows_ + 1 >= rowCapacity_ && !ExpandRow()) {
                return false;
            }
        }

        // Check for EOF
        if (ch == 0 || ch == 255) {
            break;
        }
    }

    return true;
}

// ===== Name Lookup Methods =====

int Arad_Script::GetIdx4Col(const char* colName) const {
    auto it = colNameToIndex_.find(colName);
    if (it != colNameToIndex_.end()) {
        return it->second;
    }
    return -1;
}

int Arad_Script::GetIdx4Row(const char* rowName) const {
    auto it = rowNameToIndex_.find(rowName);
    if (it != rowNameToIndex_.end()) {
        return it->second;
    }
    return -1;
}

std::string Arad_Script::GetColName(int col) const {
    for (const auto& pair : colNameToIndex_) {
        if (pair.second == col) {
            return pair.first;
        }
    }
    return "";
}

std::string Arad_Script::GetRowName(int row) const {
    for (const auto& pair : rowNameToIndex_) {
        if (pair.second == row) {
            return pair.first;
        }
    }
    return "";
}

// ===== GetString Methods (4 overloads) =====

const char* Arad_Script::GetString(int row, int col) const {
    if (row < 0 || row >= currentRows_ || col < 0 || col >= currentCols_) {
        return "";
    }
    return GetCellPtr(row, col);
}

const char* Arad_Script::GetString(const char* rowName, const char* colName) const {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    return GetString(row, col);
}

const char* Arad_Script::GetString(const char* rowName, int col) const {
    int row = GetIdx4Row(rowName);
    return GetString(row, col);
}

const char* Arad_Script::GetString(int row, const char* colName) const {
    int col = GetIdx4Col(colName);
    return GetString(row, col);
}

// ===== GetInt Methods (4 overloads) =====

int Arad_Script::GetInt(int row, int col) const {
    const char* str = GetString(row, col);
    return std::atoi(str);
}

int Arad_Script::GetInt(const char* rowName, const char* colName) const {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    return GetInt(row, col);
}

int Arad_Script::GetInt(const char* rowName, int col) const {
    int row = GetIdx4Row(rowName);
    return GetInt(row, col);
}

int Arad_Script::GetInt(int row, const char* colName) const {
    int col = GetIdx4Col(colName);
    return GetInt(row, col);
}

// ===== GetFloat Methods (4 overloads) =====

float Arad_Script::GetFloat(int row, int col) const {
    const char* str = GetString(row, col);
    return static_cast<float>(std::atof(str));
}

float Arad_Script::GetFloat(const char* rowName, const char* colName) const {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    return GetFloat(row, col);
}

float Arad_Script::GetFloat(const char* rowName, int col) const {
    int row = GetIdx4Row(rowName);
    return GetFloat(row, col);
}

float Arad_Script::GetFloat(int row, const char* colName) const {
    int col = GetIdx4Col(colName);
    return GetFloat(row, col);
}

// ===== GetDouble Methods (4 overloads) =====

double Arad_Script::GetDouble(int row, int col) const {
    const char* str = GetString(row, col);
    return std::atof(str);
}

double Arad_Script::GetDouble(const char* rowName, const char* colName) const {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    return GetDouble(row, col);
}

double Arad_Script::GetDouble(const char* rowName, int col) const {
    int row = GetIdx4Row(rowName);
    return GetDouble(row, col);
}

double Arad_Script::GetDouble(int row, const char* colName) const {
    int col = GetIdx4Col(colName);
    return GetDouble(row, col);
}

// ===== SetString Methods (4 overloads) =====

void Arad_Script::SetString(int row, int col, const char* value) {
    if (row < 0 || row >= currentRows_ || col < 0 || col >= currentCols_) {
        return;
    }

    char* cellPtr = GetCellPtr(row, col);
    std::strncpy(cellPtr, value, CELL_SIZE - 1);
    cellPtr[CELL_SIZE - 1] = '\0';
}

void Arad_Script::SetString(const char* rowName, const char* colName, const char* value) {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    SetString(row, col, value);
}

void Arad_Script::SetString(const char* rowName, int col, const char* value) {
    int row = GetIdx4Row(rowName);
    SetString(row, col, value);
}

void Arad_Script::SetString(int row, const char* colName, const char* value) {
    int col = GetIdx4Col(colName);
    SetString(row, col, value);
}

// ===== SetInt Methods (4 overloads) =====

void Arad_Script::SetInt(int row, int col, int value) {
    if (row < 0 || row >= currentRows_ || col < 0 || col >= currentCols_) {
        return;
    }

    char* cellPtr = GetCellPtr(row, col);
    std::snprintf(cellPtr, CELL_SIZE, "%d", value);
}

void Arad_Script::SetInt(const char* rowName, const char* colName, int value) {
    int col = GetIdx4Col(colName);
    int row = GetIdx4Row(rowName);
    SetInt(row, col, value);
}

void Arad_Script::SetInt(const char* rowName, int col, int value) {
    int row = GetIdx4Row(rowName);
    SetInt(row, col, value);
}

void Arad_Script::SetInt(int row, const char* colName, int value) {
    int col = GetIdx4Col(colName);
    SetInt(row, col, value);
}
