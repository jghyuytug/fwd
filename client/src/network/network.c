/**
 * @file network.c
 * @brief Network client implementation
 */

#include "../../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#define SOCKET_ERROR_CODE WSAGetLastError()
#define WOULD_BLOCK WSAEWOULDBLOCK
#define close closesocket
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define SOCKET_ERROR_CODE errno
#define WOULD_BLOCK EWOULDBLOCK
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

/* Packet builder */
struct PacketBuilder {
    u8      buffer[NET_PACKET_MAX];
    int     offset;
    u16     msg_no;
};

static PacketBuilder g_builder;

/* Write little-endian values */
static void write_le16(u8* buf, u16 value)
{
    buf[0] = (u8)(value & 0xFF);
    buf[1] = (u8)((value >> 8) & 0xFF);
}

static void write_le32(u8* buf, u32 value)
{
    buf[0] = (u8)(value & 0xFF);
    buf[1] = (u8)((value >> 8) & 0xFF);
    buf[2] = (u8)((value >> 16) & 0xFF);
    buf[3] = (u8)((value >> 24) & 0xFF);
}

/* Read little-endian values */
static u16 read_le16(const u8* buf)
{
    return (u16)buf[0] | ((u16)buf[1] << 8);
}

static u32 read_le32(const u8* buf)
{
    return (u32)buf[0] | ((u32)buf[1] << 8) |
           ((u32)buf[2] << 16) | ((u32)buf[3] << 24);
}

/* Calculate checksum */
static u32 calculate_checksum(const u8* data, int len)
{
    u32 sum = 0;
    int i;

    for (i = 0; i < len; i++) {
        sum += data[i];
    }

    return sum;
}

/* Set socket non-blocking */
static int set_nonblocking(int fd)
{
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

int network_init(NetworkClient* client)
{
    if (!client) return CLIENT_ERROR;

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return CLIENT_ERROR_NET;
    }
#endif

    memset(client, 0, sizeof(NetworkClient));
    client->socket = INVALID_SOCKET;
    client->state = NET_STATE_DISCONNECTED;

    return CLIENT_OK;
}

void network_cleanup(NetworkClient* client)
{
    if (!client) return;

    network_disconnect(client);

#ifdef _WIN32
    WSACleanup();
#endif
}

int network_connect(NetworkClient* client, const char* host, u16 port, u32 timeout_ms)
{
    struct sockaddr_in addr;
    fd_set wfds;
    struct timeval tv;
    int ret, err;
    socklen_t errlen;

    if (!client || !host || port == 0) {
        return CLIENT_ERROR;
    }

    if (client->state == NET_STATE_CONNECTED) {
        return CLIENT_OK;
    }

    /* Store connection info */
    strncpy(client->host, host, sizeof(client->host) - 1);
    client->port = port;

    /* Create socket */
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed\n");
        return CLIENT_ERROR_NET;
    }

    /* Set non-blocking */
    set_nonblocking(client->socket);

    /* Setup address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address: %s\n", host);
        close(client->socket);
        client->socket = INVALID_SOCKET;
        return CLIENT_ERROR;
    }

    client->state = NET_STATE_CONNECTING;
    printf("Connecting to %s:%u...\n", host, port);

    /* Connect (non-blocking) */
    ret = connect(client->socket, (struct sockaddr*)&addr, sizeof(addr));

#ifdef _WIN32
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
#else
    if (ret < 0 && errno != EINPROGRESS)
#endif
    {
        fprintf(stderr, "connect() failed\n");
        close(client->socket);
        client->socket = INVALID_SOCKET;
        client->state = NET_STATE_DISCONNECTED;
        return CLIENT_ERROR_NET;
    }

    /* Wait for connection */
    FD_ZERO(&wfds);
    FD_SET(client->socket, &wfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select((int)client->socket + 1, NULL, &wfds, NULL, &tv);

    if (ret <= 0) {
        fprintf(stderr, "Connection timeout\n");
        close(client->socket);
        client->socket = INVALID_SOCKET;
        client->state = NET_STATE_DISCONNECTED;
        return CLIENT_ERROR_NET;
    }

    /* Check for connect error */
    err = 0;
    errlen = sizeof(err);
    getsockopt(client->socket, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);

    if (err != 0) {
        fprintf(stderr, "Connection error\n");
        close(client->socket);
        client->socket = INVALID_SOCKET;
        client->state = NET_STATE_DISCONNECTED;
        return CLIENT_ERROR_NET;
    }

    /* Set TCP_NODELAY */
    {
        int opt = 1;
        setsockopt(client->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
    }

    client->state = NET_STATE_CONNECTED;
    client->sequence = 0;

    printf("Connected to %s:%u\n", host, port);

    if (client->on_connect) {
        client->on_connect(client);
    }

    return CLIENT_OK;
}

void network_disconnect(NetworkClient* client)
{
    if (!client) return;

    if (client->socket != INVALID_SOCKET) {
        close(client->socket);
        client->socket = INVALID_SOCKET;
    }

    if (client->state == NET_STATE_CONNECTED && client->on_disconnect) {
        client->on_disconnect(client);
    }

    client->state = NET_STATE_DISCONNECTED;
    client->recv_len = 0;
}

bool network_is_connected(NetworkClient* client)
{
    return client && client->state == NET_STATE_CONNECTED &&
           client->socket != INVALID_SOCKET;
}

int network_update(NetworkClient* client)
{
    fd_set rfds;
    struct timeval tv;
    int ret;
    int packets = 0;
    PacketHeader header;
    int total_len;

    if (!client || !network_is_connected(client)) {
        return 0;
    }

    /* Check for data */
    FD_ZERO(&rfds);
    FD_SET(client->socket, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = NET_RECV_TIMEOUT * 1000;

    ret = select((int)client->socket + 1, &rfds, NULL, NULL, &tv);

    if (ret <= 0) {
        return 0;
    }

    /* Receive data */
    ret = recv(client->socket,
               (char*)(client->recv_buffer + client->recv_len),
               NET_BUFFER_SIZE - client->recv_len, 0);

    if (ret <= 0) {
        if (ret == 0 || (SOCKET_ERROR_CODE != WOULD_BLOCK)) {
            printf("Connection lost\n");
            network_disconnect(client);
            return -1;
        }
        return 0;
    }

    client->recv_len += ret;
    client->bytes_recv += ret;

    /* Process packets */
    while (client->recv_len >= 13) {
        /* Parse header */
        header.classification = client->recv_buffer[0];
        header.length = read_le32(client->recv_buffer + 1);
        header.msg_no = read_le16(client->recv_buffer + 5);
        header.checksum = read_le32(client->recv_buffer + 7);
        header.sequence = read_le16(client->recv_buffer + 11);

        total_len = 13 + header.length;

        if (client->recv_len < total_len) {
            break;  /* Incomplete packet */
        }

        /* Verify checksum */
        u32 calc_checksum = calculate_checksum(client->recv_buffer + 13, header.length);
        if (calc_checksum != header.checksum) {
            fprintf(stderr, "Checksum mismatch!\n");
        }

        /* Dispatch packet */
        if (client->on_packet) {
            client->on_packet(client, header.msg_no,
                            client->recv_buffer + 13, header.length);
        }

        client->packets_recv++;
        packets++;

        /* Remove processed packet */
        if (client->recv_len > total_len) {
            memmove(client->recv_buffer,
                   client->recv_buffer + total_len,
                   client->recv_len - total_len);
        }
        client->recv_len -= total_len;
    }

    return packets;
}

int network_send(NetworkClient* client, u16 msg_no, const u8* data, int len)
{
    u8 packet[NET_PACKET_MAX + 13];
    int total_len;
    int sent;
    u32 checksum;

    if (!client || len < 0 || len > NET_PACKET_MAX) {
        return CLIENT_ERROR;
    }

    if (!network_is_connected(client)) {
        return CLIENT_ERROR_NET;
    }

    /* Build header */
    packet[0] = 0;  /* classification */
    write_le32(packet + 1, (u32)len);
    write_le16(packet + 5, msg_no);

    /* Copy payload */
    if (len > 0 && data) {
        memcpy(packet + 13, data, len);
    }

    /* Calculate and write checksum */
    checksum = calculate_checksum(packet + 13, len);
    write_le32(packet + 7, checksum);
    write_le16(packet + 11, client->sequence++);

    total_len = 13 + len;

    /* Send */
    sent = send(client->socket, (char*)packet, total_len, 0);
    if (sent != total_len) {
        fprintf(stderr, "Send failed\n");
        network_disconnect(client);
        return CLIENT_ERROR_NET;
    }

    client->packets_sent++;
    client->bytes_sent += total_len;

    return CLIENT_OK;
}

PacketBuilder* network_begin_packet(NetworkClient* client, u16 msg_no)
{
    (void)client;

    memset(&g_builder, 0, sizeof(g_builder));
    g_builder.msg_no = msg_no;
    g_builder.offset = 0;

    return &g_builder;
}

void packet_write_u8(PacketBuilder* builder, u8 value)
{
    if (!builder || builder->offset + 1 > NET_PACKET_MAX) return;
    builder->buffer[builder->offset++] = value;
}

void packet_write_u16(PacketBuilder* builder, u16 value)
{
    if (!builder || builder->offset + 2 > NET_PACKET_MAX) return;
    write_le16(builder->buffer + builder->offset, value);
    builder->offset += 2;
}

void packet_write_u32(PacketBuilder* builder, u32 value)
{
    if (!builder || builder->offset + 4 > NET_PACKET_MAX) return;
    write_le32(builder->buffer + builder->offset, value);
    builder->offset += 4;
}

void packet_write_i32(PacketBuilder* builder, i32 value)
{
    packet_write_u32(builder, (u32)value);
}

void packet_write_string(PacketBuilder* builder, const char* str, int max_len)
{
    int len;

    if (!builder || !str) {
        packet_write_u16(builder, 0);
        return;
    }

    len = (int)strlen(str);
    if (len > max_len) len = max_len;

    packet_write_u16(builder, (u16)len);
    packet_write_bytes(builder, (const u8*)str, len);
}

void packet_write_bytes(PacketBuilder* builder, const u8* data, int len)
{
    if (!builder || !data || len <= 0) return;
    if (builder->offset + len > NET_PACKET_MAX) return;

    memcpy(builder->buffer + builder->offset, data, len);
    builder->offset += len;
}

int network_end_packet(NetworkClient* client, PacketBuilder* builder)
{
    if (!client || !builder) return CLIENT_ERROR;

    return network_send(client, builder->msg_no, builder->buffer, builder->offset);
}
