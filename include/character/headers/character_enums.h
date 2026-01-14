#ifndef CHARACTER_CHARACTER_ENUMS_H
#define CHARACTER_CHARACTER_ENUMS_H

// ENUM_CHARAC_MANAGER_REQUEST_SUCCESS_IDX - Character manager request result codes
// Confidence: 35% (no direct evidence found, speculative based on naming)
// Evidence: None found in grep results
// Impact: 1,104 errors
// Note: Common request result codes in DNF character management
enum ENUM_CHARAC_MANAGER_REQUEST_SUCCESS_IDX {
    CHARAC_REQ_SUCCESS = 0,                 // Request successful
    CHARAC_REQ_FAIL_UNKNOWN = 1,            // Unknown failure
    CHARAC_REQ_FAIL_NOT_FOUND = 2,          // Character not found
    CHARAC_REQ_FAIL_LOCKED = 3,             // Character locked
    CHARAC_REQ_FAIL_IN_USE = 4,             // Character in use
    CHARAC_REQ_FAIL_PERMISSION = 5,         // Permission denied
    CHARAC_REQ_FAIL_INVALID = 6,            // Invalid request
    CHARAC_REQ_FAIL_DB_ERROR = 7,           // Database error

    // Values speculative based on common DNF patterns
    // Extend as discovered from packet handler analysis
};

#endif // CHARACTER_CHARACTER_ENUMS_H
