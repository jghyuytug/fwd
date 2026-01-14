/**
 * @file ipc_client.h
 * @brief IPC client for inter-service communication
 *
 * Non-blocking IPC client with automatic reconnection.
 */

#ifndef ENGINE_IPC_IPC_CLIENT_H
#define ENGINE_IPC_IPC_CLIENT_H

#include "../core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_BUFFER_SIZE     8192
#define IPC_CONNECT_TIMEOUT 5000    /* ms */
#define IPC_RECV_TIMEOUT    5000    /* ms */
#define IPC_MAX_MSG_SIZE    4096

/* IPC service types */
typedef enum {
    IPC_SERVICE_UNKNOWN = 0,
    IPC_SERVICE_GAME,
    IPC_SERVICE_GUILD,
    IPC_SERVICE_AUCTION,
    IPC_SERVICE_MONITOR,
    IPC_SERVICE_MANAGER,
    IPC_SERVICE_POINT,
    IPC_SERVICE_RELAY,
    IPC_SERVICE_MAX
} IPCServiceType;

/* IPC message header */
typedef struct {
    u16     length;     /* Message length (excluding header) */
    u8      msg_type;   /* Message type */
} IPCHeader;

/* IPC client state */
typedef enum {
    IPC_STATE_DISCONNECTED = 0,
    IPC_STATE_CONNECTING,
    IPC_STATE_CONNECTED
} IPCClientState;

/* IPC client */
typedef struct {
    int             fd;
    IPCClientState  state;
    IPCServiceType  service_type;
    char            host[64];
    u16             port;
    u32             connect_time;
    u32             last_send;
    u32             last_recv;
    u8              recv_buffer[IPC_BUFFER_SIZE];
    int             recv_len;
    /* Statistics */
    u64             messages_sent;
    u64             messages_recv;
    u64             reconnect_count;
} IPCClient;

/**
 * Initialize IPC client
 * @param client  IPC client instance
 * @param type    Service type
 * @param host    Remote host
 * @param port    Remote port
 * @return ENGINE_OK on success
 */
int ipc_client_init(IPCClient* client, IPCServiceType type,
                    const char* host, u16 port);

/**
 * Cleanup IPC client
 * @param client IPC client instance
 */
void ipc_client_cleanup(IPCClient* client);

/**
 * Connect to remote service
 * @param client     IPC client instance
 * @param timeout_ms Connection timeout
 * @return ENGINE_OK on success
 */
int ipc_client_connect(IPCClient* client, u32 timeout_ms);

/**
 * Disconnect from remote service
 * @param client IPC client instance
 */
void ipc_client_disconnect(IPCClient* client);

/**
 * Check if connected
 * @param client IPC client instance
 * @return 1 if connected, 0 otherwise
 */
int ipc_client_is_connected(const IPCClient* client);

/**
 * Send message
 * @param client   IPC client instance
 * @param msg_type Message type
 * @param data     Message data
 * @param len      Data length
 * @return ENGINE_OK on success
 */
int ipc_client_send(IPCClient* client, u8 msg_type,
                    const u8* data, int len);

/**
 * Receive message (non-blocking)
 * @param client   IPC client instance
 * @param msg_type Output: message type
 * @param data     Output buffer
 * @param max_len  Buffer size
 * @return Message length, 0 if no message, negative on error
 */
int ipc_client_recv(IPCClient* client, u8* msg_type,
                    u8* data, int max_len);

/**
 * Send and wait for response
 * @param client     IPC client instance
 * @param msg_type   Message type to send
 * @param send_data  Data to send
 * @param send_len   Send data length
 * @param resp_type  Output: response type
 * @param resp_data  Response buffer
 * @param max_len    Response buffer size
 * @param timeout_ms Timeout
 * @return Response length, negative on error
 */
int ipc_client_request(IPCClient* client, u8 msg_type,
                       const u8* send_data, int send_len,
                       u8* resp_type, u8* resp_data, int max_len,
                       u32 timeout_ms);

/**
 * Get service type name
 * @param type Service type
 * @return Type name string
 */
const char* ipc_service_name(IPCServiceType type);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_IPC_IPC_CLIENT_H */
