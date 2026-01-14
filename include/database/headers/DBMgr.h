#ifndef __DBMGR_H__
#define __DBMGR_H__

#include "MySQL.h"
#include "db_types.h"
#include "db_enums.h"
#include "CQueryCounter.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

// Opaque pointer for database manager
typedef void DBMgr;

// ========================================================================
// Function Declarations (14 functions - Pure C Interface)
// ========================================================================

// setUserIdx - Set current user index context
// Parameters:
//   this - Pointer to DBMgr instance
//   id   - User identifier/index
// Returns:
//   DBMgr* - Pointer to self (for chaining)
void *__cdecl DBMgr_setUserIdx(void *this, int id);

// activateGlobalDB - Activate global database context
// Parameters:
//   this - Pointer to DBMgr instance
// Returns:
//   DBMgr* - Pointer to self (for chaining)
void *__cdecl DBMgr_activateGlobalDB(void *this);

// Constructor - Initialize DBMgr object
// Parameters:
//   this - Pointer to DBMgr instance
void __cdecl DBMgr_DBMgr(void *this);

// Open (variant 1) - Open database connection by handle indices
// Parameters:
//   a1    - Pointer to server group/handle array
//   id    - Server group ID
//   name  - Connection name/identifier
//   src   - Connection source (hostname/address)
//   a5    - Username or port
//   a6    - Password or database name
//   value - Additional parameter (pool size or timeout)
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl DBMgr_Open_v1(_DWORD *a1, int id, char *name, char *src, char *a5, char *a6, int value);

// Open (variant 2) - Open database connection with unsigned int parameter
// Parameters:
//   a1    - Pointer to server group/handle array
//   id    - Server group ID
//   name  - Connection name/identifier
//   value - Unsigned value (port or other parameter)
//   src   - Connection source
//   a6    - Additional parameter
//   a7    - Additional parameter
//   count - Connection pool size
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl DBMgr_Open_v2(_DWORD *a1, int id, char *name, unsigned int value, char *src, char *a6, char *a7, int count);

// Close (variant 1) - Close all connections
// Parameters:
//   this - Pointer to DBMgr instance
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl DBMgr_Close_v1(void *this);

// Close (variant 2) - Close specific connection
// Parameters:
//   a1  - Pointer to server group/handle array
//   id  - Server group ID or database handle index
// Returns:
//   MySQL* - Pointer to MySQL connection (or NULL)
MySQL *__cdecl DBMgr_Close_v2(_DWORD *a1, int id);

// GetServerType - Determine server type for given context
// Parameters:
//   this - Pointer to DBMgr instance
//   ...  - Variable arguments (server group, handle index, etc.)
// Returns:
//   int - Server type code
int __cdecl DBMgr_GetServerType(void *this, ...);

// addUserDBInfo - Register new user database information
// Parameters:
//   id   - User identifier
//   flag - Database type/category flag
//   a3   - Pointer to server group or handle array
// Returns:
//   _DWORD* - Pointer to allocated info structure
_DWORD *__cdecl DBMgr_addUserDBInfo(int id, char flag, _DWORD *a3);

// removeUserDBInfo - Unregister user database information
// Parameters:
//   this - Pointer to DBMgr instance
//   ...  - Variable arguments (user ID, etc.)
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl DBMgr_removeUserDBInfo(void *this, ...);

// GetDBHandleServerGroup - Get server group for database handle
// Parameters:
//   a1    - Pointer to handle array
//   id    - Server group ID
//   value - Database handle index
// Returns:
//   int - Server group identifier
int __cdecl DBMgr_GetDBHandleServerGroup(_DWORD *a1, int id, int value);

// SetQueryCounterPointer - Associate query counter with manager
// Parameters:
//   this - Pointer to DBMgr instance
//   a2   - Pointer to CQueryCounter instance
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl DBMgr_SetQueryCounterPointer(void *this, void *a2);

// GetDBHandle - Get MySQL connection handle
// Parameters:
//   a1    - Pointer to handle array or context
//   id    - Server group ID
//   value - Database handle index
// Returns:
//   int - Handle ID or error code
int __cdecl DBMgr_GetDBHandle(int *a1, int id, int value);

// isActiveGlobalDB - Check if global database is active
// Parameters:
//   this - Pointer to DBMgr instance
// Returns:
//   int - 1=active, 0=inactive
int __cdecl DBMgr_isActiveGlobalDB(void *this);

#endif // __DBMGR_H__
