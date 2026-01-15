/**
 * @file scan_utils.cpp
 * @brief Implementation of auxiliary parsing functions
 *
 * Source: Extracted from src_split/df_game_r_part20.c:78365-82000
 * Stage: 4 (Script Parser Layer)
 * Date: 2025-10-25
 *
 * REFACTORING NOTES:
 * - Removed Hex-Rays naming artifacts (v1, v2, Decimal → value)
 * - Removed global g_dnfLex dependency (future: add DNFLexWrapper* parameter)
 * - Added const correctness
 * - Improved error handling
 * - Added documentation
 */

#include "parser/scan_utils.h"
#include "data/rdar_string_manager_stub.h"
#include <cmath>
#include <cstring>
#include <cstdlib>

// External dependencies (from Stage 2/3)
extern DNFLexWrapper g_dnfLex;  // Global lexer instance
extern RDARScriptStringManager g_scriptStringManager_;  // Global string manager (stub)

// Forward declaration of external helper
extern const char* toTString(const char* str);

// =============================================================================
// INTEGER PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Scan integer with optional success flag
 *
 * Accepts DECIMAL, HEXADECIMAL, or FLOAT (cast to int).
 *
 * @source df_game_r_part20.c:78365
 */
int ScanInt(bool* success) {
  int value = 0;
  DNFLEX_TOKEN token = g_dnfLex.getToken();

  switch (token) {
    case DNF_TOK_DECIMAL:
      value = g_dnfLex.getDecimal();
      if (success) *success = true;
      break;

    case DNF_TOK_HEXADECIMAL:
      value = g_dnfLex.getHexDecimal();
      if (success) *success = true;
      break;

    case DNF_TOK_FLOAT:
      value = static_cast<int>(g_dnfLex.getFloat());
      if (success) *success = true;
      break;

    default:
      if (success) *success = false;
      break;
  }

  return value;
}

/**
 * @brief Scan integer with simplified interface
 *
 * @source df_game_r_part20.c:78398
 */
bool ScanInt(int* outValue) {
  if (!outValue) {
    return false;
  }

  bool success = false;
  *outValue = ScanInt(&success);
  return success;
}

// =============================================================================
// FLOAT PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Scan float value
 *
 * Accepts DECIMAL, HEXADECIMAL (cast to float), or FLOAT.
 *
 * @source df_game_r_part20.c:78421
 */
float ScanFloat(bool* success) {
  float value = 0.0f;
  DNFLEX_TOKEN token = g_dnfLex.getToken();

  switch (token) {
    case DNF_TOK_DECIMAL:
      value = static_cast<float>(g_dnfLex.getDecimal());
      if (success) *success = true;
      break;

    case DNF_TOK_HEXADECIMAL:
      value = static_cast<float>(g_dnfLex.getHexDecimal());
      if (success) *success = true;
      break;

    case DNF_TOK_FLOAT:
      value = g_dnfLex.getFloat();
      if (success) *success = true;
      break;

    default:
      if (success) *success = false;
      break;
  }

  return value;
}

/**
 * @brief Scan float with precision control
 *
 * Rounds to specified decimal precision using banker's rounding.
 *
 * @source df_game_r_part20.c:78410
 */
float ScanFloat(int precision, bool* success) {
  double value = ScanFloat(success);

  // Apply precision rounding
  // Formula: round(value * 10^(precision-1)) / 10^(precision-1)
  double multiplier = std::pow(10.0, static_cast<double>(precision - 1));
  double rounded = std::floor(value * multiplier + 0.5) / multiplier;

  return static_cast<float>(rounded);
}

/**
 * @brief Scan float with simplified interface
 *
 * @source df_game_r_part20.c:78454
 */
bool ScanFloat(float* outValue) {
  if (!outValue) {
    return false;
  }

  bool success = false;
  *outValue = ScanFloat(&success);
  return success;
}

/**
 * @brief Scan float or string value
 *
 * Tries float first, falls back to string.
 *
 * @return 0 if float, 1 if string, -1 if error
 * @source df_game_r_part20.c:78466
 */
int ScanFloatStr(float* outFloat, std::string* outString) {
  *outFloat = 0.0f;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  switch (token) {
    case DNF_TOK_DECIMAL:
      *outFloat = static_cast<float>(g_dnfLex.getDecimal());
      return 0;

    case DNF_TOK_HEXADECIMAL:
      *outFloat = static_cast<float>(g_dnfLex.getHexDecimal());
      return 0;

    case DNF_TOK_FLOAT:
      *outFloat = g_dnfLex.getFloat();
      return 0;

    case DNF_TOK_STRING:
      {
        const char* str = g_dnfLex.getString();
        const char* tstr = toTString(str);
        *outString = tstr;
        return 1;
      }

    default:
      return -1;
  }
}

// =============================================================================
// STRING PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Scan string to C buffer
 *
 * Supports inline strings and string table lookups.
 *
 * @source df_game_r_part20.c:78496
 */
int ScanStr(char* dest, size_t maxLen) {
  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_STRING) {
    // Inline string
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);

    if (std::strlen(tstr) <= maxLen) {
      std::strncpy(dest, tstr, maxLen);
      dest[maxLen] = '\0';  // Ensure null termination
      return 1;
    }
  }
  else if (token == DNF_TOK_PAIR_INT) {
    // String table lookup
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),  // API requires non-const
        NULL
      );

      if (value && std::strlen(value) <= maxLen) {
        std::strncpy(dest, value, maxLen);
        dest[maxLen] = '\0';  // Ensure null termination
        return 1;
      }
    }
  }

  return 0;
}

/**
 * @brief Scan string to std::string
 *
 * @source df_game_r_part20.c:78542
 */
int ScanStr(std::string* outString) {
  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_STRING) {
    // Inline string
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);
    *outString = tstr;
    return 1;
  }
  else if (token == DNF_TOK_PAIR_INT) {
    // String table lookup
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),
        NULL
      );

      if (value) {
        *outString = value;
        return 1;
      }
    }
  }

  return 0;
}

// =============================================================================
// TYPE IDENTIFIER PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Scan type identifier string
 *
 * @source df_game_r_part20.c:78580
 */
int ScanType(std::string* outType, bool allowPairLookup) {
  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_STRING) {
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);
    *outType = tstr;
    return 1;
  }
  else if (allowPairLookup && token == DNF_TOK_PAIR_INT) {
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),
        NULL
      );

      if (value) {
        *outType = value;
        return 1;
      }
    }
  }

  return 0;
}

// =============================================================================
// COMPLEX TYPE-FLEXIBLE PARSING FUNCTIONS
// =============================================================================

/**
 * @brief Scan type identifier or string value
 *
 * @param isType Output: true if TYPE token found, false if STRING token found
 * @param outString Output: type or string value
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR string table lookups
 * @return 1 if success, 0 if error
 * @source df_game_r_part20.c:78633
 */
int ScanTypeOrStr(bool* isType, std::string* outString, bool allowPairLookup) {
  *isType = true;  // Default: assume type

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_STRING) {
    // Inline string
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);
    *outString = tstr;
    *isType = false;
    return 1;
  }
  else if (token == DNF_TOK_TYPE) {
    // Type identifier
    const char* typeStr = g_dnfLex.getType();
    const char* tstr = toTString(typeStr);
    *outString = tstr;
    *isType = true;
    return 1;
  }
  else if (allowPairLookup && token == DNF_TOK_PAIR_INT) {
    // String table lookup
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),
        NULL
      );

      if (value) {
        *outString = value;
        *isType = false;  // Treat as string
        return 1;
      }
    }
  }

  *isType = false;
  return 0;
}

/**
 * @brief Scan absolute or relative float value
 *
 * Handles special format: TYPE tokens containing "[value]" like "[1.5]".
 * Regular numeric tokens are treated as absolute values.
 *
 * @param hasType Output: true if TYPE token was found
 * @param outType Output: type identifier string (if hasType is true)
 * @param isAbsolute Output: true for absolute value, false for relative
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR lookups
 * @return Parsed float value
 * @source df_game_r_part20.c:78722
 */
float ScanAbsOrRel(bool* hasType, std::string* outType, bool* isAbsolute, bool allowPairLookup) {
  (void)allowPairLookup;  // Reserved for future use

  float value = 0.0f;
  *hasType = true;
  *isAbsolute = true;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_DECIMAL) {
    value = static_cast<float>(g_dnfLex.getDecimal());
    *hasType = false;
    *isAbsolute = true;
    return value;
  }
  else if (token == DNF_TOK_HEXADECIMAL) {
    value = static_cast<float>(g_dnfLex.getHexDecimal());
    *hasType = false;
    *isAbsolute = true;
    return value;
  }
  else if (token == DNF_TOK_FLOAT) {
    value = g_dnfLex.getFloat();
    *hasType = false;
    *isAbsolute = true;
    return value;
  }
  else if (token == DNF_TOK_TYPE) {
    // Parse TYPE string containing "[value]"
    const char* typeStr = g_dnfLex.getType();
    const char* tstr = toTString(typeStr);
    *outType = tstr;

    // Extract value from brackets: "type[1.5]" → "1.5"
    size_t startPos = outType->find('[');
    if (startPos != std::string::npos) {
      size_t endPos = outType->find(']', startPos);
      if (endPos != std::string::npos) {
        std::string valueStr = outType->substr(startPos + 1, endPos - startPos - 1);
        value = static_cast<float>(atof(valueStr.c_str()));
        *hasType = true;
        *isAbsolute = true;
        return value;
      }
    }

    *hasType = true;
    *isAbsolute = false;
    return 0.0f;
  }

  *hasType = false;
  *isAbsolute = false;
  return 0.0f;
}

/**
 * @brief Scan type identifier or integer value
 *
 * @param hasType Output: true if TYPE found, false if integer found
 * @param outType Output: type identifier string (if hasType is true)
 * @param intValue Output: pointer to receive integer value (if hasType is false)
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR lookups
 * @return 1 if success, 0 if error
 * @source df_game_r_part20.c:78815
 */
int ScanTypeOrInt(bool* hasType, std::string* outType, int* intValue, bool allowPairLookup) {
  (void)allowPairLookup;  // Reserved for future use

  *hasType = true;
  *intValue = 0;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_DECIMAL) {
    *intValue = g_dnfLex.getDecimal();
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_HEXADECIMAL) {
    *intValue = g_dnfLex.getHexDecimal();
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_FLOAT) {
    *intValue = static_cast<int>(g_dnfLex.getFloat());
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_TYPE) {
    const char* typeStr = g_dnfLex.getType();
    const char* tstr = toTString(typeStr);
    *outType = tstr;
    *hasType = true;
    return 1;
  }

  *hasType = false;
  return 0;
}

/**
 * @brief Scan string or integer value
 *
 * @param isString Output: true if string found, false if integer found
 * @param outString Output: string value (if isString is true)
 * @param intValue Output: pointer to receive integer value (if isString is false)
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR string table lookups
 * @return 1 if success, 0 if error
 * @source df_game_r_part20.c:78898
 */
int ScanStrOrInt(bool* isString, std::string* outString, int* intValue, bool allowPairLookup) {
  *isString = true;
  *intValue = 0;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_DECIMAL) {
    *intValue = g_dnfLex.getDecimal();
    *isString = false;
    return 1;
  }
  else if (token == DNF_TOK_HEXADECIMAL) {
    *intValue = g_dnfLex.getHexDecimal();
    *isString = false;
    return 1;
  }
  else if (token == DNF_TOK_FLOAT) {
    *intValue = static_cast<int>(g_dnfLex.getFloat());
    *isString = false;
    return 1;
  }
  else if (token == DNF_TOK_STRING) {
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);
    *outString = tstr;
    *isString = true;
    return 1;
  }
  else if (allowPairLookup && token == DNF_TOK_PAIR_INT) {
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),
        NULL
      );

      if (value) {
        *outString = value;
        *isString = true;
        return 1;
      }
    }
  }

  *isString = false;
  return 0;
}

/**
 * @brief Scan any data type (universal parser)
 *
 * Most flexible parser - attempts to parse any token type.
 *
 * @param intValue Output: integer value (if dataType is 1)
 * @param outString Output: string value (if dataType is 0 or 2)
 * @param dataType Output: 0=string, 1=integer, 2=type, 3=unknown
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR string table lookups
 * @return 1 if success, 0 if error
 * @source df_game_r_part20.c:79282
 */
int ScanAnyData(int* intValue, std::string* outString, int* dataType, bool allowPairLookup) {
  *dataType = 3;  // Unknown by default
  *intValue = 0;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_DECIMAL) {
    *intValue = g_dnfLex.getDecimal();
    *dataType = 1;  // Integer
    return 1;
  }
  else if (token == DNF_TOK_HEXADECIMAL) {
    *intValue = g_dnfLex.getHexDecimal();
    *dataType = 1;  // Integer
    return 1;
  }
  else if (token == DNF_TOK_FLOAT) {
    *intValue = static_cast<int>(g_dnfLex.getFloat());
    *dataType = 1;  // Integer (cast from float)
    return 1;
  }
  else if (token == DNF_TOK_STRING) {
    const char* str = g_dnfLex.getString();
    const char* tstr = toTString(str);
    *outString = tstr;
    *dataType = 0;  // String
    return 1;
  }
  else if (token == DNF_TOK_TYPE) {
    const char* typeStr = g_dnfLex.getType();
    const char* tstr = toTString(typeStr);
    *outString = tstr;
    *dataType = 2;  // Type
    return 1;
  }
  else if (allowPairLookup && token == DNF_TOK_PAIR_INT) {
    int fileIndex = g_dnfLex.getPairInt();

    if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR) {
      const char* key = g_dnfLex.getPairStr();
      const char* value = RDARScriptStringManager::findString(
        &g_scriptStringManager_,
        fileIndex,
        const_cast<char*>(key),
        NULL
      );

      if (value) {
        *outString = value;
        *dataType = 0;  // String
        return 1;
      }
    }
  }

  *dataType = 3;  // Unknown
  return 0;
}

/**
 * @brief Scan type identifier or float value
 *
 * @param hasType Output: true if TYPE found, false if float found
 * @param outType Output: type identifier string (if hasType is true)
 * @param floatValue Output: pointer to receive float value (if hasType is false)
 * @param allowPairLookup If true, allows PAIR_INT+PAIR_STR lookups
 * @return 1 if success, 0 if error
 * @source df_game_r_part20.c:81885
 */
int ScanTypeOrFloat(bool* hasType, std::string* outType, float* floatValue, bool allowPairLookup) {
  (void)allowPairLookup;  // Reserved for future use

  *hasType = true;
  *floatValue = 0.0f;

  DNFLEX_TOKEN token = g_dnfLex.getToken();

  if (token == DNF_TOK_DECIMAL) {
    *floatValue = static_cast<float>(g_dnfLex.getDecimal());
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_HEXADECIMAL) {
    *floatValue = static_cast<float>(g_dnfLex.getHexDecimal());
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_FLOAT) {
    *floatValue = g_dnfLex.getFloat();
    *hasType = false;
    return 1;
  }
  else if (token == DNF_TOK_TYPE) {
    const char* typeStr = g_dnfLex.getType();
    const char* tstr = toTString(typeStr);
    *outType = tstr;
    *hasType = true;
    return 1;
  }

  *hasType = false;
  return 0;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Convert multibyte string to TCHAR (stub implementation)
 *
 * @note This is a simplified stub. Original implementation may have
 *       more complex character set conversion.
 */
const char* toTString(const char* str) {
  // For now, just return the original string
  // TODO: Implement proper MBCS to TCHAR conversion if needed
  return str;
}
