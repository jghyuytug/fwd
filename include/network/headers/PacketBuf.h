#ifndef __PACKETBUF_H__
#define __PACKETBUF_H__

#include <stdint.h>
#include <sys/socket.h>
#include <stdbool.h>
#include "common/defs.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

// PacketBuf is defined in CHackAnalyzer.h as a struct
typedef struct PacketBuf PacketBuf;

typedef void Inven_Item;
typedef void CUser;
typedef void CItem;

// ========================================================================
// Function Declarations (50 functions - Pure C Interface)
// Note: Removed duplicate/overloaded functions for C compatibility
// ========================================================================

// Constructor/Destructor
void __cdecl PacketBuf_PacketBuf(PacketBuf *self);

// Basic Operations
PacketBuf *__cdecl PacketBuf_clear(PacketBuf *self);
PacketBuf *__cdecl PacketBuf_clear_ptr(PacketBuf *self);
PacketBuf *__cdecl PacketBuf_set_index(PacketBuf *self, int id);
int __cdecl PacketBuf_get_index(PacketBuf *self);
int __cdecl PacketBuf_at(PacketBuf *self, int id);

// Header and Finalization
int __cdecl PacketBuf_put_header(PacketBuf *self, char flag, int16_t a3);
void __cdecl PacketBuf_finalize(PacketBuf *self, bool a2);
void __cdecl PacketBuf_encFinalize(PacketBuf *self);
int __cdecl PacketBuf_is_finallized(PacketBuf *self);

// Put Functions (Basic Types)
int __cdecl PacketBuf_put_byte(PacketBuf *self, char flag);
int __cdecl PacketBuf_put_short(PacketBuf *self, int16_t a2);
int __cdecl PacketBuf_put_int(PacketBuf *self, int id);
int __cdecl PacketBuf_put_str(PacketBuf *self, char *a2, int id);
int __cdecl PacketBuf_put_binary(PacketBuf *self, const char *a2, int id);
int __cdecl PacketBuf_put_item_idx(PacketBuf *self, unsigned int id);
int __cdecl PacketBuf_put_packet(PacketBuf *self, const Inven_Item *item);

// Put Functions with Pointer (renamed to avoid conflicts)
int __cdecl PacketBuf_put_byte_ptr(PacketBuf *self, int *a2, char flag);
int __cdecl PacketBuf_put_short_ptr(PacketBuf *self, int *a2, int16_t a3);
int __cdecl PacketBuf_put_int_ptr(PacketBuf *self, int *a2, int id);
int __cdecl PacketBuf_put_str_ptr(PacketBuf *self, int *a2, char *a3, int id);
int __cdecl PacketBuf_put_binary_ptr(PacketBuf *self, int *a2, const char *a3, int id);

// Get Functions
int __cdecl PacketBuf_get_byte(PacketBuf *self, char *a2);
int __cdecl PacketBuf_get_short(PacketBuf *self, int16_t *a2);
int __cdecl PacketBuf_get_int(PacketBuf *self, int *a2);
int __cdecl PacketBuf_get_str(PacketBuf *self, char *a2, int id, int value);
int __cdecl PacketBuf_get_binary(PacketBuf *self, char *a2, int id);
int __cdecl PacketBuf_get_item_idx(PacketBuf *self, unsigned int *a2);
int __cdecl PacketBuf_get_packet(PacketBuf *self, int id);

// Packet Management
void __cdecl PacketBuf_set_packet(void **self, const char *src, int n);
int __cdecl PacketBuf_bind_packet(PacketBuf *self, char *a2, int id);
void *__cdecl PacketBuf_copy(PacketBuf *self, const PacketBuf *a2);

// Encryption and Compression
void __cdecl PacketBuf_encrypt_packet(PacketBuf *self);
int __cdecl PacketBuf_isEncryptRequired(PacketBuf *self);
int __cdecl PacketBuf_compress_packet(PacketBuf *self);
_BOOL4 __cdecl PacketBuf_isCompressRequired(PacketBuf *self, char *a2);

// Buffer Management
int __cdecl PacketBuf_get_buf_ptr(PacketBuf *self, int id);
int __cdecl PacketBuf_get_len(PacketBuf *self);
int __cdecl PacketBuf_is_ptr_end(PacketBuf *self, int id, int value);
_BOOL4 __cdecl PacketBuf_is_len_end(PacketBuf *self, int id);

// Error Handling
int __cdecl PacketBuf_setLastError(int id, int value);
int __cdecl PacketBuf_getLastError(PacketBuf *self);

#endif // __PACKETBUF_H__
