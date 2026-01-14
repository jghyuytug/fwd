/**
 * @file scan_utils.h
 * @brief Auxiliary parsing functions for DNF script parser
 *
 * This file provides generic token-to-value conversion functions that sit on top
 * of the DNFLexWrapper lexical analyzer. These functions simplify parsing by
 * handling type flexibility and error reporting.
 *
 * @note All functions use the global g_dnfLex instance by default.
 *       For custom lexer support, use the *Ex() variants (future addition).
 *
 * Source: Extracted from df_game_r_part20.c (lines 78365-82000)
 * Stage: 4 (Script Parser Layer)
 * Dependencies: Stage 3 (Lexical Analyzer)
 */

#ifndef PARSER_SCAN_UTILS_H
#define PARSER_SCAN_UTILS_H

#include <string>
#include "lexer/dnflex_wrapper.h"
#include "lexer/token_types.h"

/**
 * @brief Scan an integer value from the token stream
 *
 * Accepts DECIMAL, HEXADECIMAL, or FLOAT tokens (FLOAT cast to int).
 *
 * @param success Optional pointer to receive success status (true if valid token)
 * @return Parsed integer value, or 0 if parsing failed
 *
 * @example
 * bool ok;
 * int value = ScanInt(&ok);
 * if (ok) { // use value }
 */
int ScanInt(bool* success);

/**
 * @brief Scan an integer value with simplified interface
 *
 * @param outValue Pointer to receive parsed integer
 * @return true if parsing succeeded, false otherwise
 *
 * @example
 * int value;
 * if (ScanInt(&value)) { // use value }
 */
bool ScanInt(int* outValue);

/**
 * @brief Scan an integer value without error tracking
 *
 * Convenience overload for code that doesn't need success status
 * (success status is discarded).
 *
 * @return Parsed integer value, or 0 if parsing failed
 *
 * @example
 * int value = ScanInt();  // No error tracking
 */
inline int ScanInt() {
    bool dummy = false;
    return ScanInt(&dummy);
}

/**
 * @brief Scan a float value from the token stream
 *
 * Accepts DECIMAL, HEXADECIMAL, or FLOAT tokens.
 *
 * @param success Optional pointer to receive success status
 * @return Parsed float value, or 0.0 if parsing failed
 */
float ScanFloat(bool* success);

/**
 * @brief Scan a float value with precision control
 *
 * Rounds the result to specified decimal precision.
 *
 * @param precision Number of decimal digits (e.g., 2 for 0.01 precision)
 * @param success Optional pointer to receive success status
 * @return Parsed float value rounded to precision
 *
 * @example
 * bool ok;
 * float value = ScanFloat(2, &ok);  // Rounds to 0.01 precision
 */
float ScanFloat(int precision, bool* success);

/**
 * @brief Scan a float value with simplified interface
 *
 * @param outValue Pointer to receive parsed float
 * @return true if parsing succeeded, false otherwise
 */
bool ScanFloat(float* outValue);

/**
 * @brief Scan a float or string value
 *
 * Tries to parse as float first. If string token found, stores in outString.
 *
 * @param outFloat Pointer to receive float value (if numeric token)
 * @param outString Pointer to receive string value (if string token)
 * @return 0 if float parsed, 1 if string parsed, -1 if error
 */
int ScanFloatStr(float* outFloat, std::string* outString);

/**
 * @brief Scan a string value to C buffer
 *
 * Supports both DNF_TOK_STRING (inline string) and DNF_TOK_PAIR_INT+DNF_TOK_PAIR_STR
 * (string table lookup via RDARScriptStringManager).
 *
 * @param dest Destination buffer
 * @param maxLen Maximum buffer size (including null terminator)
 * @return 1 if success, 0 if error
 *
 * @warning Buffer overflow protection: string length must be <= maxLen
 */
int ScanStr(char* dest, size_t maxLen);

/**
 * @brief Scan a string value to std::string
 *
 * Supports both inline strings and string table lookups.
 *
 * @param outString Pointer to receive string value
 * @return 1 if success, 0 if error
 */
int ScanStr(std::string* outString);

/**
 * @brief Scan a type identifier string
 *
 * Parses string tokens representing type names (e.g., "STAT_STRENGTH").
 *
 * @param outType Pointer to receive type identifier string
 * @param allowPairLookup If true, allows string table lookups (PAIR_INT+PAIR_STR)
 * @return 1 if success, 0 if error
 */
int ScanType(std::string* outType, bool allowPairLookup);

/**
 * @brief Scan a type identifier or plain string
 *
 * More flexible version of ScanType that accepts any string format.
 *
 * @param isType Output: true if token is a type identifier, false if plain string
 * @param outString Pointer to receive string value
 * @param allowPairLookup If true, allows string table lookups
 * @return 1 if success, 0 if error
 */
int ScanTypeOrStr(bool* isType, std::string* outString, bool allowPairLookup);

/**
 * @brief Scan an absolute or relative float value
 *
 * Supports format: "TYPE value" where TYPE indicates absolute/relative.
 * Also supports plain numeric values.
 *
 * @param hasType Output: true if type prefix found
 * @param outType Output: type identifier string (if hasType is true)
 * @param isAbsolute Output: true for absolute value, false for relative
 * @param allowPairLookup If true, allows string table lookups for type
 * @return Parsed float value
 *
 * @example
 * // Parse "+10%" or "ABS 100"
 * bool hasType, isAbs;
 * std::string type;
 * float value = ScanAbsOrRel(&hasType, &type, &isAbs, true);
 */
float ScanAbsOrRel(bool* hasType, std::string* outType, bool* isAbsolute, bool allowPairLookup);

/**
 * @brief Scan a type identifier or integer value
 *
 * @param hasType Output: true if type found, false if integer found
 * @param outType Output: type identifier string (if hasType is true)
 * @param intValue Output: integer value (if hasType is false)
 * @param allowPairLookup If true, allows string table lookups
 * @return 1 if success, 0 if error
 */
int ScanTypeOrInt(bool* hasType, std::string* outType, int* intValue, bool allowPairLookup);

/**
 * @brief Scan a string or integer value
 *
 * @param isString Output: true if string found, false if integer found
 * @param outString Output: string value (if isString is true)
 * @param intValue Output: integer value (if isString is false)
 * @param allowPairLookup If true, allows string table lookups
 * @return 1 if success, 0 if error
 */
int ScanStrOrInt(bool* isString, std::string* outString, int* intValue, bool allowPairLookup);

/**
 * @brief Scan any data type (most flexible parser)
 *
 * Attempts to parse as integer, string, or type identifier.
 *
 * @param intValue Output: integer value (if numeric token)
 * @param outString Output: string value (if string token)
 * @param dataType Output: 0=integer, 1=string, 2=type
 * @param allowPairLookup If true, allows string table lookups
 * @return 1 if success, 0 if error
 */
int ScanAnyData(int* intValue, std::string* outString, int* dataType, bool allowPairLookup);

/**
 * @brief Scan a type identifier or float value
 *
 * @param hasType Output: true if type found, false if float found
 * @param outType Output: type identifier string (if hasType is true)
 * @param floatValue Output: float value (if hasType is false)
 * @param allowPairLookup If true, allows string table lookups
 * @return 1 if success, 0 if error
 */
int ScanTypeOrFloat(bool* hasType, std::string* outType, float* floatValue, bool allowPairLookup);

// --- Internal Helper Functions (not for public use) ---

/**
 * @brief Convert multibyte string to TCHAR string (internal use)
 * @internal
 */
const char* toTString(const char* str);

#endif // PARSER_SCAN_UTILS_H
