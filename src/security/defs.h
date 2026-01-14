/*
 * defs.h - Common type definitions for game server
 * Provides standard type definitions and helper macros
 */

#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include <stdint.h>
#include <sys/types.h>

// Basic integer types
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// IDA-style type names for compatibility
typedef uint8_t  _BYTE;
typedef uint16_t _WORD;
typedef uint32_t _DWORD;
typedef uint64_t _QWORD;

// Boolean type variations
typedef int8_t _BOOL1;
typedef int16_t _BOOL2;
typedef int32_t _BOOL4;

// Pointer-sized types
#if defined(__LP64__) || defined(_WIN64)
typedef uint64_t _QWORD_PTR;
typedef int64_t _SQWORD_PTR;
#else
typedef uint32_t _DWORD_PTR;
typedef int32_t _SDWORD_PTR;
#endif

// Helper macros for bitwise operations
#ifndef LOBYTE
#define LOBYTE(w)  ((uint8_t)(w))
#define HIBYTE(w)  ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define LOWORD(d)  ((uint16_t)(d))
#define HIWORD(d)  ((uint16_t)(((uint32_t)(d) >> 16) & 0xFFFF))
#define LODWORD(q) ((uint32_t)(q))
#define HIDWORD(q) ((uint32_t)(((uint64_t)(q) >> 32) & 0xFFFFFFFF))
#endif

// Alignment and packing macros
#ifndef ALIGN
#define ALIGN(x) __attribute__((aligned(x)))
#endif

#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

// Common game server constants
#define MAX_PACKET_SIZE      65536
#define MAX_NAME_LENGTH      128
#define MAX_DESCRIPTION_LEN  256
#define MAX_ERROR_MSG_LEN    512

// Error code type
typedef int32_t error_code_t;

#endif /* __COMMON_DEFS_H__ */