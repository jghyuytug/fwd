#ifndef __MYSQL_H__
#define __MYSQL_H__

// ========================================================================
// Database Layer - MySQL Wrapper Interface
// Minimal includes to avoid conflicts with system headers
// ========================================================================

#include <stdint.h>
#include <stdbool.h>

// ========================================================================
// Type Definitions (from IDA decompiler types)
// ========================================================================

// Integer types
typedef uint8_t   _BYTE;
typedef int8_t    __int8;
typedef uint16_t  _WORD;
typedef int16_t   __int16;
typedef uint32_t  _DWORD;
typedef int32_t   __int32;
typedef uint64_t  _QWORD;
typedef int64_t   __int64;

// Boolean types
typedef bool      _BOOL1;
typedef int16_t   _BOOL2;
typedef int32_t   _BOOL4;

// Calling conventions
#ifndef __cdecl
  #if defined(__GNUC__) || defined(__clang__)
    #define __cdecl __attribute__((cdecl))
  #else
    #define __cdecl
  #endif
#endif

#ifndef __fastcall
  #if defined(__GNUC__) || defined(__clang__)
    #define __fastcall __attribute__((fastcall))
  #else
    #define __fastcall
  #endif
#endif

// ========================================================================
// Forward Declarations
// ========================================================================

// Opaque pointers to avoid including system mysql.h
typedef struct MySQL_opaque MySQL;
typedef struct st_mysql MYSQL_SYSTEM;
typedef struct st_mysql_res MYSQL_RES;
typedef struct st_mysql_row MYSQL_ROW;
typedef unsigned long MYSQL_FIELD_OFFSET;
typedef struct st_mysql_field MYSQL_FIELD;

// Game types
struct CQueryCounter;

// ========================================================================
// MySQL Class Method Declarations (49 functions)
//
// These are declared as C-style functions but operate on MySQL objects.
// The implementation in MySQL.cpp uses C++ member syntax but these
// declarations match the decompiled calling convention.
// ========================================================================

// Constructor/Destructor (implicit)
// void __cdecl MySQL::MySQL(MySQL *this);

// Connection Management
extern int __cdecl MySQL_init(MySQL *pthis);
extern int __cdecl MySQL_init_db_handle(MySQL *pthis);
extern int __cdecl MySQL_open_v1(MySQL *pthis, char *name, char *src, char *a4, char *a5);
extern int __cdecl MySQL_open_v2(MySQL *pthis, char *name, unsigned int id, char *src, char *a5, char *a6);
extern MySQL *__cdecl MySQL_close(MySQL *pthis);
extern int __cdecl MySQL_ping(MySQL *pthis);

// Configuration
extern int __cdecl MySQL_set_compress_option(MySQL *pthis);
extern int __cdecl MySQL_set_read_default_grp_option(MySQL *pthis);
extern int MySQL_set_charset_name_option();
extern int MySQL_set_reconnect_option();

// Query Methods
extern int __cdecl MySQL_set_query_fmt(MySQL *pthis, char *format, ...);
extern int __cdecl MySQL_set_query(MySQL *pthis);
extern int __cdecl MySQL_exec(MySQL *pthis, bool a2);
extern int __cdecl MySQL_exec_query(MySQL *pthis);
extern int __cdecl MySQL_select(MySQL *pthis);
extern int __cdecl MySQL_select_fmt(MySQL *pthis, char *format, ...);
extern int __cdecl MySQL_insert(MySQL *pthis, char *format, ...);
extern int __cdecl MySQL_update(MySQL *pthis, char *format, ...);
extern int __cdecl MySQL_fetch(MySQL *pthis);

// Field Accessors
extern int __cdecl MySQL_get_int_u(MySQL *pthis, int id, unsigned int *a3);
extern int __cdecl MySQL_get_int(MySQL *pthis, int id, int *a3);
extern int __cdecl MySQL_get_uint(MySQL *pthis, int id, unsigned int *a3);
extern int __cdecl MySQL_get_str(MySQL *pthis, int id, char *dest, int n);
extern int __cdecl MySQL_get_short_i16(MySQL *pthis, int id, __int16 *a3);
extern int __cdecl MySQL_get_short_i32(MySQL *pthis, int id, int *a3);
extern int __cdecl MySQL_get_byte_char(MySQL *pthis, int id, char *a3);
extern int __cdecl MySQL_get_byte_int(MySQL *pthis, int id, int *a3);
extern int __cdecl MySQL_get_ubyte(MySQL *pthis, int id, unsigned __int8 *a3);
extern int __cdecl MySQL_get_ushort(MySQL *pthis, int id, unsigned __int16 *a3);
extern int __cdecl MySQL_get_ulong(MySQL *pthis, int id, unsigned int *a3);
extern int __cdecl MySQL_get_ulonglong(MySQL *pthis, int id, unsigned __int64 *a3);
extern int __cdecl MySQL_get_longlong(MySQL *pthis, int id, __int64 *a3);
extern int __cdecl MySQL_get_float(MySQL *pthis, int id, float *a3);
extern int __cdecl MySQL_get_char(MySQL *pthis, int id, char *a3);
extern int __cdecl MySQL_get_binary(MySQL *pthis, int id, void *s, int n);
extern int __cdecl MySQL_get_binary_length(MySQL *pthis, int id);

// Validation & Metadata
extern _BOOL4 __cdecl MySQL_is_valid_col(MySQL *pthis, int id);
extern _BOOL4 __cdecl MySQL_is_valid_data(MySQL *pthis, int id);
extern int __cdecl MySQL_get_n_rows(MySQL *pthis);

// Utility
extern int __cdecl MySQL_getDBError(MySQL *pthis);
extern int __cdecl MySQL_getServerVersion(MySQL *pthis);
extern int __cdecl MySQL_GetQueryCounter(MySQL *pthis);
extern MySQL *__cdecl MySQL_SetQueryCounter(MySQL **pthis, CQueryCounter *a2);
extern char *__cdecl MySQL_blob_to_str(MySQL *pthis, unsigned int id, void *a3, int value);
extern MySQL *__cdecl MySQL_clear_result_set(MySQL *pthis);
extern char *__cdecl MySQL_GetQueryString(MySQL *pthis);
extern int __cdecl MySQL_escape_string(MySQL *pthis, char *a2, char *s);
extern __int64 __cdecl MySQL_getAffectedRowCount(MySQL *pthis);

#endif // __MYSQL_H__
