/**
 * @file game_server.h
 * @brief DNF Game Server - Real Network Implementation
 *
 * Implements actual TCP server for client connections:
 * - epoll-based event loop
 * - Packet parsing with tagPacketHeader
 * - Session management
 * - Protocol handlers
 *
 * @version 1.0
 * @date 2025-11-26
 */

#ifndef SRC_NETWORK_GAME_SERVER_H_
#define SRC_NETWORK_GAME_SERVER_H_

#include <stdint.h>
#include "network/headers/packet_enums.h"

/*==============================================================================
 * Configuration
 *============================================================================*/

#define SERVER_DEFAULT_PORT         7000
#define SERVER_MAX_CONNECTIONS      4096
#define SERVER_RECV_BUFFER_SIZE     65536
#define SERVER_SEND_BUFFER_SIZE     65536
#define SERVER_BACKLOG              128
#define PACKET_MAX_SIZE             65535

/*==============================================================================
 * Protocol Header Configuration (from siroco*.cfg)
 *
 * header_classification = 0 1   // offset 0, 1 byte
 * header_sLength = 1 4          // offset 1, 4 bytes
 * header_msg_no = 5 1           // offset 5, 1 byte
 * (padding at offset 6, 1 byte)
 * header_checksum = 7 4         // offset 7, 4 bytes
 * header_sequence = 11 2        // offset 11, 2 bytes
 * header_mangled_length = 13    // total header size
 *============================================================================*/
#define PACKET_HEADER_SIZE          13
#define PACKET_HEADER_SIZE_SIMPLE   4   /* Legacy 4-byte header for compatibility */

/*==============================================================================
 * Data Structures
 *============================================================================*/

/**
 * Legacy packet header (4 bytes, for simple testing)
 */
typedef struct {
    uint16_t length;    /* Total packet length (including header) */
    uint16_t type;      /* Packet type (ENUM_CMDPACKET) */
} PacketHeaderSimple;

/**
 * Full packet header (13 bytes, from production server config)
 *
 * Structure based on siroco*.cfg:
 * - classification: 1 byte at offset 0 (packet classification/type)
 * - sLength: 4 bytes at offset 1 (packet length, little-endian)
 * - msg_no: 1 byte at offset 5 (message number/command)
 * - padding: 1 byte at offset 6 (reserved)
 * - checksum: 4 bytes at offset 7 (CRC32 or custom checksum)
 * - sequence: 2 bytes at offset 11 (packet sequence number)
 */
typedef struct __attribute__((packed)) {
    uint8_t  classification;  /* Offset 0: Packet classification */
    uint32_t length;          /* Offset 1: Packet length (little-endian) */
    uint8_t  msg_no;          /* Offset 5: Message number (command type) */
    uint8_t  padding;         /* Offset 6: Reserved/padding */
    uint32_t checksum;        /* Offset 7: Packet checksum */
    uint16_t sequence;        /* Offset 11: Sequence number */
} PacketHeader;

/* Verify header size at compile time */
_Static_assert(sizeof(PacketHeader) == 13, "PacketHeader must be 13 bytes");

/**
 * Client session state
 */
typedef enum {
    SESSION_STATE_CONNECTED = 0,    /* Just connected, not authenticated */
    SESSION_STATE_AUTHENTICATED,    /* Logged in successfully */
    SESSION_STATE_CHAR_SELECT,      /* At character selection */
    SESSION_STATE_IN_GAME,          /* Playing in game */
    SESSION_STATE_DISCONNECTING     /* Being disconnected */
} SessionState;

/**
 * Client session data
 */
typedef struct ClientSession {
    int fd;                                 /* Socket file descriptor */
    uint32_t ip_addr;                       /* Client IP (network order) */
    uint16_t port;                          /* Client port */
    SessionState state;                     /* Current state */

    /* Receive buffer */
    uint8_t recv_buffer[SERVER_RECV_BUFFER_SIZE];
    int recv_len;                           /* Data in recv buffer */

    /* Send buffer */
    uint8_t send_buffer[SERVER_SEND_BUFFER_SIZE];
    int send_len;                           /* Data in send buffer */
    int send_pos;                           /* Sent position */

    /* User info (after login) */
    uint32_t account_id;
    uint32_t character_id;
    char account_name[32];
    char character_name[16];

    /* Statistics */
    uint64_t bytes_received;
    uint64_t bytes_sent;
    uint32_t packets_received;
    uint32_t packets_sent;
    uint32_t connect_time;
    uint32_t last_activity;

    /* Flags */
    int is_active;
} ClientSession;

/**
 * Server statistics
 */
typedef struct {
    uint32_t total_connections;
    uint32_t current_connections;
    uint32_t peak_connections;
    uint64_t total_bytes_received;
    uint64_t total_bytes_sent;
    uint64_t total_packets_processed;
    uint32_t login_attempts;
    uint32_t login_successes;
    uint32_t start_time;
} ServerStats;

/**
 * Packet handler callback
 */
typedef int (*PacketHandlerFunc)(ClientSession* session, const uint8_t* data, int len);

/*==============================================================================
 * Server API
 *============================================================================*/

/**
 * Initialize game server
 *
 * @param port Port to listen on (0 for default)
 * @return 0 on success, -1 on error
 */
int GameServer_Initialize(int port);

/**
 * Start server main loop (blocking)
 *
 * @return 0 on clean shutdown, -1 on error
 */
int GameServer_Run(void);

/**
 * Stop server
 */
void GameServer_Stop(void);

/**
 * Cleanup server resources
 */
void GameServer_Cleanup(void);

/**
 * Get server statistics
 *
 * @param stats Output statistics structure
 */
void GameServer_GetStats(ServerStats* stats);

/**
 * Send packet to client
 *
 * @param session Client session
 * @param type Packet type
 * @param data Packet payload (after header)
 * @param len Payload length
 * @return 0 on success, -1 on error
 */
int GameServer_SendPacket(ClientSession* session, uint16_t type, const void* data, int len);

/**
 * Disconnect client
 *
 * @param session Client session
 * @param reason Disconnect reason code
 */
void GameServer_DisconnectClient(ClientSession* session, int reason);

/**
 * Register custom packet handler
 *
 * @param packet_type Packet type to handle
 * @param handler Handler function
 * @return 0 on success, -1 on error
 */
int GameServer_RegisterHandler(uint16_t packet_type, PacketHandlerFunc handler);

/*==============================================================================
 * Session API
 *============================================================================*/

/**
 * Get session by file descriptor
 *
 * @param fd Socket file descriptor
 * @return Session pointer or NULL
 */
ClientSession* GameServer_GetSession(int fd);

/**
 * Get active session count
 *
 * @return Number of active sessions
 */
int GameServer_GetSessionCount(void);

/**
 * Iterate all active sessions
 *
 * @param callback Function to call for each session
 * @param context User context pointer
 */
void GameServer_ForEachSession(void (*callback)(ClientSession* session, void* context), void* context);

/*==============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * Get packet type name for debugging
 *
 * @param type Packet type
 * @return String name or "UNKNOWN"
 */
const char* GameServer_GetPacketTypeName(uint16_t type);

/**
 * Format IP address to string
 *
 * @param ip IP address (network order)
 * @param buffer Output buffer
 * @param size Buffer size
 * @return buffer pointer
 */
char* GameServer_FormatIP(uint32_t ip, char* buffer, int size);

/*==============================================================================
 * Protocol Utility Functions
 *============================================================================*/

/**
 * Calculate packet checksum (CRC32-based)
 *
 * @param data Data buffer
 * @param len Data length
 * @return Checksum value
 */
uint32_t GameServer_CalculateChecksum(const uint8_t* data, int len);

/**
 * Verify packet checksum
 *
 * @param header Packet header
 * @param payload Packet payload
 * @param payload_len Payload length
 * @return 1 if valid, 0 if invalid
 */
int GameServer_VerifyChecksum(const PacketHeader* header, const uint8_t* payload, int payload_len);

/**
 * Set protocol mode
 *
 * @param use_full_header 1 to use 13-byte header, 0 for legacy 4-byte header
 */
void GameServer_SetProtocolMode(int use_full_header);

/**
 * Get current protocol header size
 *
 * @return Header size in bytes (4 or 13)
 */
int GameServer_GetHeaderSize(void);

#endif /* SRC_NETWORK_GAME_SERVER_H_ */
