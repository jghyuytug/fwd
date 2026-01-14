/**
 * @file types.h
 * @brief Engine core type definitions
 *
 * GCC 4.1.2 compatible, C89/C99 standard
 */

#ifndef ENGINE_CORE_TYPES_H
#define ENGINE_CORE_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Boolean type for C89 compatibility */
#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

/* Fixed-width integer types */
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

/* Pointer-sized integer (32-bit) */
typedef uint32_t  uptr;
typedef int32_t   iptr;

/* Result codes */
typedef enum {
    ENGINE_OK           =  0,
    ENGINE_ERROR        = -1,
    ENGINE_ERROR_PARAM  = -2,
    ENGINE_ERROR_MEMORY = -3,
    ENGINE_ERROR_IO     = -4,
    ENGINE_ERROR_TIMEOUT= -5,
    ENGINE_ERROR_FULL   = -6,
    ENGINE_ERROR_EMPTY  = -7,
    ENGINE_ERROR_EXISTS = -8,
    ENGINE_ERROR_NOTFOUND = -9
} EngineResult;

/* Common limits */
#define ENGINE_MAX_PATH     256
#define ENGINE_MAX_NAME     64
#define ENGINE_MAX_ERROR    256

/* Alignment macros */
#define ENGINE_ALIGN(x, a)  (((x) + (a) - 1) & ~((a) - 1))
#define ENGINE_ALIGN4(x)    ENGINE_ALIGN(x, 4)
#define ENGINE_ALIGN8(x)    ENGINE_ALIGN(x, 8)

/* Min/Max macros */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* Array size macro */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Unused parameter */
#define UNUSED(x) (void)(x)

/* Likely/Unlikely hints */
#ifdef __GNUC__
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

/* Inline hint */
#ifdef __GNUC__
#define INLINE static __inline__
#else
#define INLINE static
#endif

/* Export/Import */
#ifdef _WIN32
#define ENGINE_EXPORT __declspec(dllexport)
#define ENGINE_IMPORT __declspec(dllimport)
#else
#define ENGINE_EXPORT __attribute__((visibility("default")))
#define ENGINE_IMPORT
#endif

#ifdef ENGINE_BUILD
#define ENGINE_API ENGINE_EXPORT
#else
#define ENGINE_API ENGINE_IMPORT
#endif

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_TYPES_H */
