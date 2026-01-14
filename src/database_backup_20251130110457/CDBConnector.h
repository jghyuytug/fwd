#ifndef __CDBCONNECTOR_H__
#define __CDBCONNECTOR_H__

#include "MySQL.h"
#include "db_types.h"
#include "db_enums.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

// Opaque pointers to avoid internal implementation details
typedef void CDBConnector;
typedef void DBMgr;

// ========================================================================
// Function Declarations (9 functions - Pure C Interface)
// ========================================================================

// Constructor - Initialize CDBConnector object
// Parameters:
//   this - Pointer to CDBConnector instance
//   a2   - Pointer to DBMgr for database management
void __cdecl CDBConnector_CDBConnector(void *this, void *a2);

// Init - Initialize database connection pool
// Parameters:
//   this - Pointer to CDBConnector instance
//   a2   - Pointer to connection info (STDBConnInfo)
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl CDBConnector_Init(void *this, void *a2);

// Close - Close all database connections
// Returns:
//   MySQL* - Pointer to MySQL connection object (or NULL)
MySQL *__cdecl CDBConnector_Close(void);

// OpenMaster - Open connection to master database server
// Parameters:
//   this - Pointer to CDBConnector instance
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl CDBConnector_OpenMaster(void *this);

// Open - Open connection to database by handle index
// Parameters:
//   id    - Server group ID (ENUM_SERVER_GROUP)
//   value - Database handle index (ENUM_DB_HANDLE_IDX)
//   count - Connection pool size
//   name  - Connection name/identifier
// Returns:
//   _BOOL4 - TRUE if successful, FALSE otherwise
_BOOL4 __cdecl CDBConnector_Open(int id, int value, int count, char *name);

// Connect - Establish connection with retry logic
// Parameters:
//   id    - Server group ID
//   value - Database handle index
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CDBConnector_Connect(int id, int value);

// ConnectWithServerGroup - Connect using server group context
// Parameters:
//   id    - Server group ID
//   value - Database handle index
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CDBConnector_ConnectWithServerGroup(int id, int value);

// GetConnInfo - Retrieve connection information structure
// Parameters:
//   id    - Server group ID
//   value - Database handle index
//   count - Buffer size or index
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CDBConnector_GetConnInfo(int id, int value, int count);

// QueryConnInfo - Query connection information from database
// Parameters:
//   id    - Server group ID
//   value - Database handle index
//   count - Query parameter
//   index - Result index
// Returns:
//   int - Status code (0=success, non-zero=error)
int __cdecl CDBConnector_QueryConnInfo(int id, int value, int count, int index);

#endif // __CDBCONNECTOR_H__
