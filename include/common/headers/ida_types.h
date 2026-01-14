#ifndef IDA_TYPES_H
#define IDA_TYPES_H

// IDA Pro decompiler type mappings to standard C/C++ types
// These types are used in decompiled code and need to be mapped to portable types

#include <stdint.h>
#include <stdbool.h>

// Basic integer types (IDA convention -> standard C)
typedef uint8_t   _BYTE;
typedef uint16_t  _WORD;
typedef uint32_t  _DWORD;
typedef uint64_t  _QWORD;

typedef int8_t    __int8;
typedef int16_t   __int16;
typedef int32_t   __int32;
typedef int64_t   __int64;

// Boolean types
typedef int32_t   _BOOL4;  // IDA uses 4-byte bool
typedef int32_t   BOOL;    // Win32 style BOOL

// Calling conventions (Linux ignores these, but we define for compatibility)
#ifndef __cdecl
#define __cdecl
#endif

#ifndef __stdcall
#define __stdcall
#endif

#ifndef __fastcall
#define __fastcall
#endif

// Common macros
#ifndef BYREF
#define BYREF  // IDA marker for reference parameters
#endif

#endif // IDA_TYPES_H
