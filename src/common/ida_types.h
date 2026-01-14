/**
 * IDA/Hex-Rays Type Definitions
 * Common types and structures extracted from decompiled code
 */

#ifndef INCLUDE_COMMON_IDA_TYPES_H_
#define INCLUDE_COMMON_IDA_TYPES_H_

#include <stdint.h>
#include <time.h>

/* Integer types */
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

/* Pointer size dependent */
typedef uintptr_t           size_t;
typedef intptr_t            ssize_t;

/* Common type aliases */
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long long  QWORD;

/* NULL definition */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* Min/Max macros */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Array size macro */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Function calling convention */
#define __cdecl
#define __stdcall
#define __fastcall

/* Structure packing */
#pragma pack(push, 1)
typedef struct {
    uint32_t version;
    uint32_t timestamp;
    char reserved[64];
} IDASectionHeader;
#pragma pack(pop)

#endif // INCLUDE_COMMON_IDA_TYPES_H_
