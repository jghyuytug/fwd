#ifndef __INTERFACEPACKETBUF_H__
#define __INTERFACEPACKETBUF_H__

#include <stdint.h>
#include <sys/socket.h>
#include "common/defs.h"

// Forward declarations
struct PacketBuf;
struct InterfacePacketBuf;
struct Inven_Item;

// Function declarations (26 functions - removed duplicates)

PacketBuf *__cdecl InterfacePacketBuf_clear(PacketBuf **self);
int __cdecl InterfacePacketBuf_put_header(PacketBuf **self, char flag, __int16 a3);
int __cdecl InterfacePacketBuf_put_byte(PacketBuf **self, char flag);
int __cdecl InterfacePacketBuf_put_int(PacketBuf **self, int id);
void __cdecl InterfacePacketBuf_finalize(PacketBuf **self, bool a2);
int __cdecl InterfacePacketBuf_put_short(PacketBuf **self, __int16 a2);
int __cdecl InterfacePacketBuf_put_byte_with_ptr(PacketBuf **self, int *a2, char flag);
int __cdecl InterfacePacketBuf_get_index(PacketBuf **self);
int __cdecl InterfacePacketBuf_put_short_with_ptr(PacketBuf **self, int *a2, __int16 a3);
int __cdecl InterfacePacketBuf_put_binary(PacketBuf **self, char *a2, int id);
int __cdecl InterfacePacketBuf_put_packet(PacketBuf **self, const Inven_Item *item);
int __cdecl InterfacePacketBuf_put_str(PacketBuf **self, char *a2, int id);
int __cdecl InterfacePacketBuf_get(InterfacePacketBuf *self);
int __cdecl InterfacePacketBuf_put_item_idx(PacketBuf **self, unsigned int id);
PacketBuf *__cdecl InterfacePacketBuf_set_index(PacketBuf **self, int id);
int __cdecl InterfacePacketBuf_bind_packet(PacketBuf **self, char *a2, int id);
int __cdecl InterfacePacketBuf_get_short(PacketBuf **self, __int16 *a2);
int __cdecl InterfacePacketBuf_get_int(PacketBuf **self, int *a2);
int __cdecl InterfacePacketBuf_get_binary(PacketBuf **self, char *a2, int id);
int __cdecl InterfacePacketBuf_get_packet(PacketBuf **self, int id);
int __cdecl InterfacePacketBuf_get_len(PacketBuf **self);
int __cdecl InterfacePacketBuf_is_finallized(PacketBuf **self);
int __cdecl InterfacePacketBuf_put_int_with_ptr(PacketBuf **self, int *a2, int id);
void __cdecl InterfacePacketBuf_InterfacePacketBuf(InterfacePacketBuf *self);
InterfacePacketBuf *__cdecl InterfacePacketBuf_AcquirePacketBuf(InterfacePacketBuf *self, PacketBuf *a2);
int __cdecl InterfacePacketBuf_getLastError(PacketBuf **self);

#endif // __INTERFACEPACKETBUF_H__
