#ifndef CNETWORK_H
#define CNETWORK_H

#include <sys/types.h>
#include <stdint.h>
#include "common/defs.h"

// ========================================================================
// Type Definitions (Forward Declarations for C Compatibility)
// ========================================================================

typedef void CNetwork;

// ========================================================================
// CNetwork C-Compatible Interface (19 functions)
// ========================================================================

// Constructor - Initialize network connection manager
void __cdecl CNetwork_CNetwork(void *this, int a1);

// Destructor - Clean up network resources
void __cdecl CNetwork_destructor(void *this);

// disconnect - Disconnect a client connection
int __cdecl CNetwork_disconnect(void *this, int a1);

// set_socket - Set socket descriptor for connection
int __cdecl CNetwork_set_socket(void *this, int *a1, int a2, char a3);

// SetNonBlock - Set socket to non-blocking mode
_BOOL4 __cdecl CNetwork_SetNonBlock(void *this, int fd);

// GetPeerIP2 - Get peer IP address
void *__cdecl CNetwork_GetPeerIP2(void *this, int *a1, void *s, int n);

// get_str_ip - Get string representation of IP
int __cdecl CNetwork_get_str_ip(void *this, int a1);

// get_socket - Get socket descriptor
int __cdecl CNetwork_get_socket(void *this, int a1);

// get_ip - Get IP address as integer
int __cdecl CNetwork_get_ip(void *this, int a1);

// get_port - Get port number
int __cdecl CNetwork_get_port(void *this, int a1);

// get_inner_ip - Get internal IP address
int __cdecl CNetwork_get_inner_ip(void *this, int a1);

// get_mtu - Get maximum transmission unit
int __cdecl CNetwork_get_mtu(void *this, int a1);

// get_nat_type - Get NAT type
int __cdecl CNetwork_get_nat_type(void *this, int a1);

// set_nat (variant 1) - Set NAT with int parameter
char *__cdecl CNetwork_set_nat_v1(void *this, int a1, char a2, int a3, int a4, int16_t a5, int a6);

// set_nat (variant 2) - Set NAT with string parameter
char *__cdecl CNetwork_set_nat_v2(void *this, int a1, char a2, int a3, int a4, int16_t a5, char *src);

// get_mac_addr - Get MAC address
int __cdecl CNetwork_get_mac_addr(void *this, int a1);

// send_packet (variant 1) - Send packet with default parameters
int __cdecl CNetwork_send_packet_v1(void *this, int a1);

// send_packet (variant 2) - Send packet with source data
int __cdecl CNetwork_send_packet_v2(void *this, int a1, char *src, int n);

// recv_packet - Receive packet
int __cdecl CNetwork_recv_packet(void *this, int a1);

// send_lock - Acquire send lock
int __cdecl CNetwork_send_lock(void *this, int a1);

// send_unlock - Release send lock
int __cdecl CNetwork_send_unlock(void *this, int a1);

// initNetworkBuffer - Initialize network buffer
void *__cdecl CNetwork_initNetworkBuffer(void *this, _DWORD *a1);

// GetRemainSendSize - Get remaining send buffer size
int __cdecl CNetwork_GetRemainSendSize(void *this, int a1);

#endif // CNETWORK_H
