#ifndef CSESSIONMANAGER_H
#define CSESSIONMANAGER_H

#include <sys/epoll.h>
#include <stdbool.h>

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

typedef void CSessionManager;
typedef void CSession;
typedef void CEpoll;
// PacketBuf is defined in CHackAnalyzer.h as a struct
typedef struct PacketBuf PacketBuf;
typedef void PacketGuard;
typedef void EpollHandler;

// ========================================================================
// CSessionManager C-Compatible Interface (5 functions)
// ========================================================================

// Constructor - Initialize session manager
void __cdecl CSessionManager_CSessionManager(void *this, void *a1);

// Destructor - Clean up session manager resources
void __cdecl CSessionManager_destructor(void *this);

// WaitForSessionEvent - Wait for session events via epoll
int __cdecl CSessionManager_WaitForSessionEvent(void *this, struct epoll_event *event, int timeout);

// TrySend - Attempt to send packet through session
int __cdecl CSessionManager_TrySend(void *this, void *a1, void *session, void **packet);

// TryConnect - Attempt to connect to remote address
int __cdecl CSessionManager_TryConnect(void *this, struct epoll_event *event, void *session, char *address);

// TryListen - Attempt to listen on address/port
bool __cdecl CSessionManager_TryListen(void *this, struct epoll_event *event, void *session, char *address);

// TryDisconnect - Attempt to disconnect session
int __cdecl CSessionManager_TryDisconnect(void *this, void *a1, void *session);

#endif // CSESSIONMANAGER_H
