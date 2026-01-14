#ifndef __TAGPACKETHEADER_H__
#define __TAGPACKETHEADER_H__

#include <common/defs.h>
#include <stdint.h>
#include <sys/socket.h>

// Forward declaration
struct tagPacketHeader;

// Function declarations (5 functions)

tagPacketHeader *__cdecl tagPacketHeader_setPacketID(tagPacketHeader *self, char flag);
int __cdecl tagPacketHeader_getSize(tagPacketHeader *self);
tagPacketHeader *__cdecl tagPacketHeader_setSize(tagPacketHeader *self, int id);
tagPacketHeader *__cdecl tagPacketHeader_setCategory(tagPacketHeader *self, char flag);
void __cdecl tagPacketHeader_tagPacketHeader(tagPacketHeader *self);

#endif // __TAGPACKETHEADER_H__
