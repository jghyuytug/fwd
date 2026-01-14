#ifndef __CMMCHANNELPROXY_H__
#define __CMMCHANNELPROXY_H__

#include <common/defs.h>
#include <stdint.h>
#include <sys/socket.h>

// Forward declaration
struct CmmChannelProxy;

// Function declarations (7 functions)
// Note: Using CmmChannelProxy* instead of std::string* to avoid type conflicts with defs.h

void __cdecl CmmChannelProxy_CmmChannelProxy(CmmChannelProxy *self, const char *a2, int id);
int __cdecl CmmChannelProxy_Init(CmmChannelProxy *self);
int __cdecl CmmChannelProxy_SendChannelInfo(CmmChannelProxy *self);
int __cdecl CmmChannelProxy_ConnectBridgeServer(CmmChannelProxy *self);
int __cdecl CmmChannelProxy_SendPacket(CmmChannelProxy *self, char *buf, size_t n);
int __cdecl CmmChannelProxy_RecvPacket(CmmChannelProxy *self);
int __cdecl CmmChannelProxy_SendCheckChannelScript(CmmChannelProxy *self);

#endif // __CMMCHANNELPROXY_H__
