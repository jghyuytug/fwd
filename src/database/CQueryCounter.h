#ifndef __CQUERYCOUNTER_H__
#define __CQUERYCOUNTER_H__

#include "MySQL.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

// Opaque pointer for query counter object
typedef void CQueryCounter;

// ========================================================================
// Function Declarations (10 functions - Pure C Interface)
// ========================================================================

// setLogDB - Set logging database flag
// Parameters:
//   this - Pointer to CQueryCounter instance
//   a2   - Log database flag (true=enabled, false=disabled)
// Returns:
//   CQueryCounter* - Pointer to self (for chaining)
void *__cdecl CQueryCounter_setLogDB(void *this, _BOOL4 a2);

// GetQueryCountTable - Get name of query count table
// Parameters:
//   this - Pointer to CQueryCounter instance
// Returns:
//   char* - Table name string
char *__cdecl CQueryCounter_GetQueryCountTable(void *this);

// Constructor - Initialize CQueryCounter object
// Parameters:
//   this - Pointer to CQueryCounter instance
void __cdecl CQueryCounter_CQueryCounter(void *this);

// Reset - Reset query counter state
// Parameters:
//   this - Pointer to CQueryCounter instance
// Returns:
//   CQueryCounter* - Pointer to self (for chaining)
void *__cdecl CQueryCounter_Reset(void *this);

// IncreaseQueryCount - Increment query count for named query
// Parameters:
//   this - Pointer to CQueryCounter instance
//   a2   - Query name string (C string, not std::string)
void __cdecl CQueryCounter_IncreaseQueryCount(void *this, const char *a2);

// UpdateQueryCounter - Update counter state and statistics
// Parameters:
//   this - Pointer to CQueryCounter instance
void __cdecl CQueryCounter_UpdateQueryCounter(void *this);

// SetResponseTime - Record query response time
// Parameters:
//   this - Pointer to CQueryCounter instance
//   a2   - Response time in milliseconds (double)
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl CQueryCounter_SetResponseTime(void *this, double a2);

// LoadQueryTable - Load query table from file/database
// Parameters:
//   this - Pointer to CQueryCounter instance
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CQueryCounter_LoadQueryTable(void *this);

// SaveQuery - Save query statistics to file/database
// Parameters:
//   this - Pointer to CQueryCounter instance
//   a2   - Output filename or identifier
//   a3   - Pointer to status variable
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CQueryCounter_SaveQuery(void *this, char *a2, int *a3);

// RegisterQuery - Register new query type for tracking
// Parameters:
//   this - Pointer to CQueryCounter instance
//   a2   - Query name string
//   flag - Query type/category flag
//   id   - Query identifier
// Returns:
//   int - Registration status (0=success, non-zero=error)
int __cdecl CQueryCounter_RegisterQuery(void *this, const char *a2, char flag, int id);

#endif // __CQUERYCOUNTER_H__
