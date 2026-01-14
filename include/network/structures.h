/**
 * Network Module Structure Definitions
 * Structures for packets, connections, and network protocol
 */

#ifndef INCLUDE_NETWORK_STRUCTURES_H_
#define INCLUDE_NETWORK_STRUCTURES_H_

#include <common/ida_types.h>
#include <sys/socket.h>

/* Packet structure */
typedef struct {
    uint16_t packet_size;
    uint16_t packet_id;
    uint32_t sequence;
    uint32_t timestamp;
    uint8_t data[65536];
} PacketHeader;

/* Session information */
typedef struct {
    int socket_fd;
    struct sockaddr_in client_addr;
    uint32_t session_id;
    time_t connect_time;
    time_t last_activity;
    int is_connected;
} SessionInfo;

/* Connection manager */
typedef struct {
    SessionInfo* sessions;
    int max_sessions;
    int active_sessions;
    int epoll_fd;
} CNetwork;

/* epoll wrapper */
typedef struct {
    int epoll_fd;
    struct epoll_event* events;
    int max_events;
} CEpoll;

#endif // INCLUDE_NETWORK_STRUCTURES_H_
