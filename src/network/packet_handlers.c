/*
 * Packet Handlers - Example packet handler implementations
 *
 * Week 4 Example Handlers:
 * 1. HeartbeatHandler - Handle heartbeat packets
 * 2. EchoHandler - Echo test packets
 * 3. AuthRequestHandler - Authentication request (simplified)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "network/headers/PacketDispatcher.h"
#include "network/headers/session_state.h"
#include "security/headers/CHackAnalyzer.h"

/* Database Integration for Authentication */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/d_taiwan/headers/accounts_orm.h"

/* Database manager from user_module */
extern DBConnectionManager* g_db_manager;

/* External function declarations (defined in network.c) */
extern int Session_QueueSend(int fd, const char *data, int len);
extern int PacketBuf_put_byte(PacketBuf *self, char flag);
extern int PacketBuf_put_short(PacketBuf *self, __int16 value);
extern int PacketBuf_put_int(PacketBuf *self, int value);
extern int PacketBuf_put_str(PacketBuf *self, char *str, int maxlen);
extern int PacketBuf_get_byte(PacketBuf *self, char *out_value);
extern int PacketBuf_get_short(PacketBuf *self, __int16 *out_value);
extern int PacketBuf_get_int(PacketBuf *self, int *out_value);
extern int PacketBuf_get_str(PacketBuf *self, char *out_buffer, int buffer_size, int unused);
extern void PacketBuf_finalize(PacketBuf *self, int encrypt);

/* Helper functions */

static PacketBuf* CreateResponsePacket(uint16_t packet_type, int capacity)
{
    PacketBuf *buf = (PacketBuf*)malloc(sizeof(PacketBuf));
    if (!buf) return NULL;

    buf->data = malloc(capacity);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    memset(buf->data, 0, capacity);
    buf->size = 0;
    buf->capacity = capacity;
    buf->read_pos = 0;
    buf->write_pos = 4;
    buf->finalized = 0;
    buf->encrypted = 0;
    buf->compressed = 0;
    buf->_reserved = 0;
    buf->header = buf->data;

    tagPacketHeader *header = (tagPacketHeader*)buf->header;
    header->type = packet_type;
    header->length = 4;

    buf->size = 4;

    return buf;
}

static void FreePacketBuf(PacketBuf *buf)
{
    if (buf) {
        if (buf->data) free(buf->data);
        free(buf);
    }
}

static int SendPacket(int fd, PacketBuf *packet)
{
    if (!packet || !packet->data) return -1;

    PacketBuf_finalize(packet, 0);

    int success = Session_QueueSend(fd, (const char*)packet->data, packet->size);

    return success ? 0 : -1;
}

/* Handler implementations */

/*
 * Handler_Heartbeat - Heartbeat packet handler
 * Type: 10 (PACKET_TYPE_HEARTBEAT)
 */
int Handler_Heartbeat(int fd, PacketBuf *packet)
{
    printf("[Handler_Heartbeat] Received from fd=%d\n", fd);

    // Week 5: 更新心跳时间，重置超时计时器
    Session_UpdateHeartbeat(fd);

    PacketBuf *response = CreateResponsePacket(PACKET_TYPE_HEARTBEAT, 256);
    if (!response) {
        fprintf(stderr, "[Handler_Heartbeat] Failed to create response\n");
        return -1;
    }

    time_t server_time = time(NULL);
    PacketBuf_put_int(response, (int)server_time);

    int result = SendPacket(fd, response);
    FreePacketBuf(response);

    if (result < 0) {
        fprintf(stderr, "[Handler_Heartbeat] Failed to send response\n");
        return -1;
    }

    printf("[Handler_Heartbeat] Sent response to fd=%d (time=%ld)\n", fd, (long)server_time);

    return 0;
}

/*
 * Handler_EchoTest - Echo test handler
 * Type: 65000 (PACKET_TYPE_ECHO_TEST)
 */
int Handler_EchoTest(int fd, PacketBuf *packet)
{
    tagPacketHeader *header = (tagPacketHeader*)packet->data;

    printf("[Handler_EchoTest] Received from fd=%d (length=%d)\n", fd, header->length);

    int success = Session_QueueSend(fd, (const char*)packet->data, header->length);

    if (!success) {
        fprintf(stderr, "[Handler_EchoTest] Failed to queue echo\n");
        return -1;
    }

    printf("[Handler_EchoTest] Sent echo to fd=%d\n", fd);
    return 0;
}

/*
 * Handler_AuthRequest - Authentication request handler (simplified)
 * Type: 2 (PACKET_TYPE_AUTH_REQUEST)
 */
int Handler_AuthRequest(int fd, PacketBuf *packet)
{
    char username[64];
    char password[64];

    packet->read_pos = 4;

    if (!PacketBuf_get_str(packet, username, sizeof(username), 0)) {
        fprintf(stderr, "[Handler_AuthRequest] Failed to read username\n");
        return -1;
    }

    if (!PacketBuf_get_str(packet, password, sizeof(password), 0)) {
        fprintf(stderr, "[Handler_AuthRequest] Failed to read password\n");
        return -1;
    }

    printf("[Handler_AuthRequest] Auth from fd=%d: user='%s', pass='%s'\n",
           fd, username, password);

    /* Database authentication via Accounts ORM */
    char auth_success = 0;
    int account_id = 0;

    if (g_db_manager) {
        int verified_uid = Account_Verify(g_db_manager, username, password);
        if (verified_uid > 0) {
            auth_success = 1;
            account_id = verified_uid;
            printf("[Handler_AuthRequest] DB auth success: account_id=%d\n", account_id);
        } else {
            printf("[Handler_AuthRequest] DB auth failed for user '%s'\n", username);
        }
    } else {
        /* Fallback: Accept all logins (development mode) */
        auth_success = 1;
        account_id = 1000 + fd;  /* Temporary ID based on fd */
        printf("[Handler_AuthRequest] No DB manager, using fallback auth\n");
    }

    PacketBuf *response = CreateResponsePacket(PACKET_TYPE_AUTH_RESPONSE, 256);
    if (!response) {
        return -1;
    }

    PacketBuf_put_byte(response, auth_success);

    if (auth_success) {
        // Week 5: 认证成功，更新会话状态
        if (Session_SetAuth(fd, account_id, username) < 0) {
            fprintf(stderr, "[Handler_AuthRequest] Failed to set auth info\n");
            // 继续发送响应，但记录错误
        }
    }

    int result = SendPacket(fd, response);
    FreePacketBuf(response);

    if (result < 0) {
        return -1;
    }

    printf("[Handler_AuthRequest] Sent response to fd=%d (result=%s, account_id=%d)\n",
           fd, auth_success ? "SUCCESS" : "FAILED", auth_success ? account_id : 0);

    return 0;
}

/*
 * RegisterDefaultHandlers - Register all default handlers
 */
int RegisterDefaultHandlers(void)
{
    int result;

    printf("[PacketHandlers] Registering default handlers...\n");

    result = PacketDispatcher_Register(PACKET_TYPE_HEARTBEAT, Handler_Heartbeat, "Heartbeat");
    if (result < 0) {
        fprintf(stderr, "[PacketHandlers] Failed to register Heartbeat\n");
        return -1;
    }

    result = PacketDispatcher_Register(PACKET_TYPE_ECHO_TEST, Handler_EchoTest, "EchoTest");
    if (result < 0) {
        fprintf(stderr, "[PacketHandlers] Failed to register EchoTest\n");
        return -1;
    }

    result = PacketDispatcher_Register(PACKET_TYPE_AUTH_REQUEST, Handler_AuthRequest, "AuthRequest");
    if (result < 0) {
        fprintf(stderr, "[PacketHandlers] Failed to register AuthRequest\n");
        return -1;
    }

    printf("[PacketHandlers] All handlers registered\n");
    return 0;
}
