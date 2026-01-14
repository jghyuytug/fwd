#ifndef CEPOLL_H
#define CEPOLL_H

#include <sys/epoll.h>
#include <stdbool.h>

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

typedef void CEpoll;
typedef void CSession;
typedef void EpollHandler;

// ========================================================================
// CEpoll C-Compatible Interface (5 functions)
// ========================================================================

// Constructor - Initialize epoll event loop
void __cdecl CEpoll_CEpoll(void *this, void *a1);

// Destructor - Clean up epoll resources
void __cdecl CEpoll_destructor(void *this);

// WaitForEvent - Wait for epoll events
int __cdecl CEpoll_WaitForEvent(void *this, struct epoll_event *event, int timeout);

// RegisterSession - Register session with epoll
bool __cdecl CEpoll_RegisterSession(void *this, struct epoll_event *event, void *session, int a3);

// UnregisterSession - Unregister session from epoll
bool __cdecl CEpoll_UnregisterSession(void *this, void *a1, int socket_fd);

#endif // CEPOLL_H
