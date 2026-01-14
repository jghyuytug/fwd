/*
 * defs.h - Hex-Rays Decompiler Type Definitions
 *
 * This file provides type definitions for Hex-Rays IDA decompiler output.
 * Generated for df_game_r project - DNF game server restoration.
 */

#ifndef HEXRAYS_DEFS_H
#define HEXRAYS_DEFS_H

#include <stdint.h>
#include <stdbool.h>

/* ========================================================================
 * Basic Integer Types (Hex-Rays naming convention)
 * ======================================================================== */

// 8-bit types
typedef uint8_t   _BYTE;
typedef int8_t    __int8;

// 16-bit types
typedef uint16_t  _WORD;
typedef int16_t   __int16;
typedef uint16_t  unsigned__int16;  // Unsigned variant

// 32-bit types
typedef uint32_t  _DWORD;
typedef int32_t   __int32;
typedef int32_t   int32;

// 64-bit types
typedef uint64_t  _QWORD;
typedef int64_t   __int64;

// 128-bit types (for SSE/AVX registers)
// Disabled: conflicts with compiler builtin
// typedef struct { uint64_t low, high; } __int128;


/* ========================================================================
 * Boolean Types
 * ======================================================================== */

// 8-bit boolean (C++ bool)
typedef bool      _BOOL1;

// 16-bit boolean (rare)
typedef int16_t   _BOOL2;

// 32-bit boolean (common in Win32 API and game code)
typedef int32_t   _BOOL4;


/* ========================================================================
 * Calling Conventions
 * ======================================================================== */

// Standard C calling convention (cdecl)
// - Caller cleans up stack
// - Arguments pushed right-to-left
#ifndef __cdecl
  #if defined(__GNUC__) || defined(__clang__)
    #define __cdecl __attribute__((cdecl))
  #else
    #define __cdecl
  #endif
#endif

// Fast call convention (fastcall)
// - First 2 args in ECX, EDX
// - Rest on stack
#ifndef __fastcall
  #if defined(__GNUC__) || defined(__clang__)
    #define __fastcall __attribute__((fastcall))
  #else
    #define __fastcall
  #endif
#endif

// Standard call convention (stdcall)
// - Callee cleans up stack
// - Common in Win32 API
#ifndef __stdcall
  #if defined(__GNUC__) || defined(__clang__)
    #define __stdcall __attribute__((stdcall))
  #else
    #define __stdcall
  #endif
#endif

// This call convention (thiscall)
// - Used for C++ member functions
// - 'this' pointer in ECX
#ifndef __thiscall
  #if defined(__GNUC__) || defined(__clang__)
    #define __thiscall __attribute__((thiscall))
  #else
    #define __thiscall
  #endif
#endif

// No return attribute (for functions that never return)
#ifndef __noreturn
  #if defined(__GNUC__) || defined(__clang__)
    #define __noreturn __attribute__((noreturn))
  #else
    #define __noreturn
  #endif
#endif

// User-defined calling conventions (Hex-Rays specific)
// These need to be manually handled in restored code
#ifndef __usercall
  #define __usercall  // Custom register allocation
#endif

#ifndef __userpurge
  #define __userpurge // Custom register allocation (callee cleanup)
#endif


/* ========================================================================
 * Register Pseudo-Variables
 *
 * Hex-Rays uses these to indicate return values in specific registers.
 * In real code, these should be removed and proper return types used.
 * ======================================================================== */

// Return value in EAX
#define _EAX

// Return value in EBX
#define _EBX

// Return value in ECX
#define _ECX

// Return value in EDX
#define _EDX

// Return value in ST(0) - x87 FPU register
#define _ST0

// Return value in ST(7) - x87 FPU register
#define _FST7


/* ========================================================================
 * Compiler-Specific Attributes
 * ======================================================================== */

// Weak symbol (can be overridden by strong definition)
#ifndef weak
  #if defined(__GNUC__) || defined(__clang__)
    #define weak __attribute__((weak))
  #else
    #define weak
  #endif
#endif

// Hidden symbol (not exported)
#ifndef __hidden
  #if defined(__GNUC__) || defined(__clang__)
    #define __hidden __attribute__((visibility("hidden")))
  #else
    #define __hidden
  #endif
#endif


/* ========================================================================
 * Memory Access Macros
 *
 * These macros help with pointer arithmetic and type casts
 * commonly seen in decompiled code.
 * ======================================================================== */

// Read/Write BYTE at address
#define LOBYTE(x)   (*(_BYTE*)&(x))
#define HIBYTE(x)   (*((_BYTE*)&(x)+1))

// Read/Write WORD at address
#define LOWORD(x)   (*(_WORD*)&(x))
#define HIWORD(x)   (*((_WORD*)&(x)+1))

// Read/Write DWORD at address
#define LODWORD(x)  (*(_DWORD*)&(x))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))

// Combine high and low parts
#define __PAIR64__(high, low)  (((_QWORD)(high) << 32) | (_DWORD)(low))
#define __PAIR__(high, low)    __PAIR64__(high, low)


/* ========================================================================
 * Array Size Macro
 * ======================================================================== */

#ifndef ARRAY_SIZE
  #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


/* ========================================================================
 * Compatibility Aliases
 * ======================================================================== */

// Common type aliases used in game code
typedef _DWORD DWORD;
typedef _WORD  WORD;
typedef _BYTE  BYTE;


/* ========================================================================
 * Forward Declarations for Common Types
 *
 * These are referenced throughout the code but defined elsewhere.
 * ======================================================================== */

// Standard C/C++ library types
struct _IO_FILE;
typedef struct _IO_FILE FILE;

struct tm;
struct timeval;
struct stat;
struct sockaddr;
struct epoll_event;
struct iovec;
struct in_addr;
struct rlimit;
struct dirent;

// POSIX types
typedef unsigned int __mode_t;
typedef int __pid_t;
typedef unsigned int __uid_t;
typedef long int __off_t;
typedef long long int __off64_t;
typedef unsigned int __socklen_t;
// Disabled: conflicts with system headers
// typedef int socklen_t;
typedef long int __time_t;
typedef long int time_t;
typedef unsigned int __useconds_t;
typedef int __rlimit_resource_t;

// GCC types
typedef void *__gnuc_va_list;
typedef int wint_t;
// Disabled: conflicts with C++ builtin type
// typedef int wchar_t;
typedef void *iconv_t;
typedef struct { int __val[2]; } mbstate_t;

// Thread types (disabled - conflicts with system headers in C++)
// typedef unsigned long int pthread_t;
// typedef union pthread_attr_t pthread_attr_t;
// typedef union pthread_mutex_t pthread_mutex_t;
// typedef union pthread_mutexattr_t pthread_mutexattr_t;
// typedef union pthread_cond_t pthread_cond_t;
// typedef union pthread_rwlock_t pthread_rwlock_t;
// typedef union pthread_rwlockattr_t pthread_rwlockattr_t;
// typedef unsigned int pthread_key_t;

// Signal handling
typedef void (*__sighandler_t)(int);
// Disabled: conflicts with system headers
// typedef struct __sigset_t sigset_t;

// Directory handling
typedef struct __dirstream DIR;

// Math types
typedef struct { long int quot; long int rem; } ldiv_t;

// Network types
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

// Jump buffer (disabled - conflicts with system headers)
// struct __jmp_buf_tag {
//   long int __jmpbuf[6];
//   int __mask_was_saved;
//   sigset_t __saved_mask;
// };


#endif /* HEXRAYS_DEFS_H */
