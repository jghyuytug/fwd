/*
 * defs.h - Common type definitions for Hex-Rays decompiled code
 * This file provides standard type definitions used by IDA Pro/Hex-Rays decompiler
 */

#ifndef __DEFS_H__
#define __DEFS_H__

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

// IDA-style type names
typedef uint8_t  _BYTE;
typedef uint16_t _WORD;
typedef uint32_t _DWORD;
typedef uint64_t _QWORD;

// Boolean type
typedef int8_t _BOOL1;
typedef int16_t _BOOL2;
typedef int32_t _BOOL4;

// Unknown/opaque types
typedef void _UNKNOWN;

// Pointer-sized types
#if defined(__LP64__) || defined(_WIN64)
typedef uint64_t _QWORD_PTR;
typedef int64_t _SQWORD_PTR;
#else
typedef uint32_t _DWORD_PTR;
typedef int32_t _SDWORD_PTR;
#endif

// Helper macros
#ifndef LOBYTE
#define LOBYTE(w)  ((uint8_t)(w))
#define HIBYTE(w)  ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define LOWORD(d)  ((uint16_t)(d))
#define HIWORD(d)  ((uint16_t)(((uint32_t)(d) >> 16) & 0xFFFF))
#define LODWORD(q) ((uint32_t)(q))
#define HIDWORD(q) ((uint32_t)(((uint64_t)(q) >> 32) & 0xFFFFFFFF))
#endif

// Ensure proper alignment for structures
#ifndef ALIGN
#define ALIGN(x) __attribute__((aligned(x)))
#endif

// Packing for structures
#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

#endif /* __DEFS_H__ */
