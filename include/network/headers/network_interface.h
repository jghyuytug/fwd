#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "common/defs.h"
#include "common/headers/error_codes.h"
#include "CNetwork.h"
#include "CEpoll.h"
#include "CSession.h"
#include "CSessionManager.h"
#include "PacketBuf.h"
#include "packet_types.h"
#include "packet_enums.h"

int Network_Initialize(int max_connections, int port);
void Network_Cleanup();
int Network_Start();
void Network_Stop();
int Network_SendPacket(void *session, const void *packet, int length);
int Network_GetConnectionCount();
int Network_GetMaxConnections();
void Network_SetPacketHandler(void (*handler)(void *session, const void *packet, int length));

#endif // NETWORK_INTERFACE_H
