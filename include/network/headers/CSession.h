#ifndef __CSESSION_H__
#define __CSESSION_H__

#include <stdint.h>
#include <sys/socket.h>
#include <pthread.h>
#include "common/defs.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

typedef void CSession;
// PacketBuf is defined in PacketBuf.h

// ========================================================================
// Function Declarations (16 functions - Pure C Interface)
// ========================================================================

// GetChannelNo - Get channel number from session
int __cdecl CSession_GetChannelNo(void *self);

// GetSocket - Get socket file descriptor from session
int __cdecl CSession_GetSocket(void *self);

// GetTriggerSessionEventType - Get trigger event type
int __cdecl CSession_GetTriggerSessionEventType(void *self);

// SetTriggerSessionEventType - Set trigger event type
void __cdecl CSession_SetTriggerSessionEventType(void *self, int type);

// AddTriggerSessionEventType - Add trigger event type (bitwise OR)
void __cdecl CSession_AddTriggerSessionEventType(void *self, int type);

// Parsing - Parse incoming packet data
int __cdecl CSession_Parsing(void *self, char *a2);

// OnRecv - Handle receive event
_BOOL4 __cdecl CSession_OnRecv(void *self);

// OnSend - Handle send event
_BOOL4 __cdecl CSession_OnSend(void *fd_ptr);

// OnError - Handle error event
int __cdecl CSession_OnError(void *self);

// OnAccept - Handle accept event
int *__cdecl CSession_OnAccept(void *self);

// OnConnect - Handle connect event
int __cdecl CSession_OnConnect(void *self);

// Send - Send packet to client
_BOOL4 __cdecl CSession_Send(void *self, void **a2);

// Listen - Start listening on address/port
int *__cdecl CSession_Listen(void *self, char *cp);

// Connect - Connect to remote address
int *__cdecl CSession_Connect(void *self, const char *a2);

// Disconnect - Close session connection
int __cdecl CSession_Disconnect(void *self);

// Global session manager
extern void* g_SessionManager;

#endif // __CSESSION_H__
