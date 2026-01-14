/**
 * @file dispatcher.h
 * @brief DNF Packet Dispatcher Interface
 *
 * Provides packet dispatching system based on the 41 DisPatcher
 * classes from the production server.
 *
 * @version 1.0
 * @date 2025-11-26
 */

#ifndef NETWORK_DISPATCHER_H
#define NETWORK_DISPATCHER_H

#include <stdint.h>

/* Include full session definition from game_server.h */
#include "game_server.h"

/**
 * Dispatcher_Initialize - Initialize the packet dispatcher
 *
 * @return: 0 on success, negative on failure
 *
 * Must be called before using Dispatcher_Dispatch.
 * Registers all default packet handlers.
 */
int Dispatcher_Initialize(void);

/**
 * Dispatcher_Cleanup - Clean up dispatcher resources
 *
 * Unregisters all handlers and frees resources.
 */
void Dispatcher_Cleanup(void);

/**
 * Dispatcher_Dispatch - Dispatch a packet to its handler
 *
 * @param session: Client session pointer
 * @param packet_type: Packet type from ENUM_CMDPACKET
 * @param data: Packet payload (after header)
 * @param len: Payload length
 * @return: 0 on success, negative on error (closes connection)
 *
 * Routes the packet to the appropriate handler based on packet_type.
 * Unknown packets are logged and ignored.
 */
int Dispatcher_Dispatch(ClientSession* session, uint16_t packet_type, const uint8_t* data, int len);

/**
 * Dispatcher_GetStatistics - Get dispatcher statistics
 *
 * @param handled: Output for total packets handled
 * @param unknown: Output for unknown packets received
 * @param errors: Output for handler errors
 */
void Dispatcher_GetStatistics(unsigned long* handled, unsigned long* unknown, unsigned long* errors);

/**
 * Dispatcher_PrintRegistry - Print all registered handlers (debug)
 */
void Dispatcher_PrintRegistry(void);

#endif /* NETWORK_DISPATCHER_H */
