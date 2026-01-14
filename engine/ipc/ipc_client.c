/**
 * @file ipc_client.c
 * @brief IPC client implementation
 */

#include "ipc_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

static const char* service_names[] = {
    "UNKNOWN",
    "GAME",
    "GUILD",
    "AUCTION",
    "MONITOR",
    "MANAGER",
    "POINT",
    "RELAY"
};

static u32 get_time(void)
{
    return (u32)time(NULL);
}

static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int set_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

int ipc_client_init(IPCClient* client, IPCServiceType type,
                    const char* host, u16 port)
{
    if (!client || !host || port == 0) {
        return ENGINE_ERROR_PARAM;
    }

    memset(client, 0, sizeof(IPCClient));

    client->fd = -1;
    client->state = IPC_STATE_DISCONNECTED;
    client->service_type = type;
    client->port = port;
    strncpy(client->host, host, sizeof(client->host) - 1);

    return ENGINE_OK;
}

void ipc_client_cleanup(IPCClient* client)
{
    if (!client) return;

    ipc_client_disconnect(client);
    memset(client, 0, sizeof(IPCClient));
    client->fd = -1;
}

int ipc_client_connect(IPCClient* client, u32 timeout_ms)
{
    struct sockaddr_in addr;
    fd_set wfds;
    struct timeval tv;
    int fd;
    int ret;
    int err;
    socklen_t errlen;
    int opt;

    if (!client) {
        return ENGINE_ERROR_PARAM;
    }

    if (client->state == IPC_STATE_CONNECTED) {
        return ENGINE_OK;
    }

    /* Create socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "[IPC] socket() failed: %s\n", strerror(errno));
        return ENGINE_ERROR_IO;
    }

    /* Set non-blocking for connect */
    if (set_nonblocking(fd) < 0) {
        close(fd);
        return ENGINE_ERROR_IO;
    }

    /* Setup address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(client->port);

    if (inet_pton(AF_INET, client->host, &addr.sin_addr) <= 0) {
        close(fd);
        return ENGINE_ERROR_PARAM;
    }

    client->state = IPC_STATE_CONNECTING;

    /* Connect (non-blocking) */
    ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));

    if (ret < 0 && errno != EINPROGRESS) {
        fprintf(stderr, "[IPC] connect() failed: %s\n", strerror(errno));
        close(fd);
        client->state = IPC_STATE_DISCONNECTED;
        return ENGINE_ERROR_IO;
    }

    /* Wait for connection */
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(fd + 1, NULL, &wfds, NULL, &tv);

    if (ret <= 0) {
        close(fd);
        client->state = IPC_STATE_DISCONNECTED;
        return ret == 0 ? ENGINE_ERROR_TIMEOUT : ENGINE_ERROR_IO;
    }

    /* Check for connect error */
    err = 0;
    errlen = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen);

    if (err != 0) {
        fprintf(stderr, "[IPC] Connect error: %s\n", strerror(err));
        close(fd);
        client->state = IPC_STATE_DISCONNECTED;
        return ENGINE_ERROR_IO;
    }

    /* Set blocking mode for normal operation */
    set_blocking(fd);

    /* Set TCP_NODELAY */
    opt = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    client->fd = fd;
    client->state = IPC_STATE_CONNECTED;
    client->connect_time = get_time();
    client->recv_len = 0;

    printf("[IPC] Connected to %s:%u (%s)\n",
           client->host, client->port,
           ipc_service_name(client->service_type));

    return ENGINE_OK;
}

void ipc_client_disconnect(IPCClient* client)
{
    if (!client) return;

    if (client->fd >= 0) {
        close(client->fd);
        client->fd = -1;
    }

    client->state = IPC_STATE_DISCONNECTED;
    client->recv_len = 0;
}

int ipc_client_is_connected(const IPCClient* client)
{
    return client && client->state == IPC_STATE_CONNECTED && client->fd >= 0;
}

int ipc_client_send(IPCClient* client, u8 msg_type,
                    const u8* data, int len)
{
    u8 header[3];
    u16 msg_len;
    int total;
    int sent;

    if (!client || len < 0 || len > IPC_MAX_MSG_SIZE) {
        return ENGINE_ERROR_PARAM;
    }

    if (!ipc_client_is_connected(client)) {
        return ENGINE_ERROR_IO;
    }

    msg_len = (u16)len;

    /* Build header */
    header[0] = (u8)(msg_len & 0xFF);
    header[1] = (u8)((msg_len >> 8) & 0xFF);
    header[2] = msg_type;

    /* Send header */
    sent = send(client->fd, header, 3, MSG_NOSIGNAL);
    if (sent != 3) {
        ipc_client_disconnect(client);
        return ENGINE_ERROR_IO;
    }

    /* Send data */
    if (len > 0 && data) {
        total = 0;
        while (total < len) {
            sent = send(client->fd, data + total, len - total, MSG_NOSIGNAL);
            if (sent <= 0) {
                ipc_client_disconnect(client);
                return ENGINE_ERROR_IO;
            }
            total += sent;
        }
    }

    client->messages_sent++;
    client->last_send = get_time();

    return ENGINE_OK;
}

int ipc_client_recv(IPCClient* client, u8* msg_type,
                    u8* data, int max_len)
{
    fd_set rfds;
    struct timeval tv;
    int ret;
    u16 msg_len;
    int to_read;

    if (!client || !msg_type || !data || max_len <= 0) {
        return ENGINE_ERROR_PARAM;
    }

    if (!ipc_client_is_connected(client)) {
        return ENGINE_ERROR_IO;
    }

    /* Check if data available (non-blocking) */
    FD_ZERO(&rfds);
    FD_SET(client->fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    ret = select(client->fd + 1, &rfds, NULL, NULL, &tv);

    if (ret <= 0) {
        return 0;  /* No data */
    }

    /* Read into buffer */
    ret = recv(client->fd, client->recv_buffer + client->recv_len,
               IPC_BUFFER_SIZE - client->recv_len, 0);

    if (ret <= 0) {
        if (ret == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            ipc_client_disconnect(client);
            return ENGINE_ERROR_IO;
        }
        return 0;
    }

    client->recv_len += ret;

    /* Check for complete message */
    if (client->recv_len < 3) {
        return 0;  /* Need more data */
    }

    msg_len = (u16)client->recv_buffer[0] |
              ((u16)client->recv_buffer[1] << 8);

    if (client->recv_len < 3 + msg_len) {
        return 0;  /* Need more data */
    }

    /* Complete message received */
    *msg_type = client->recv_buffer[2];

    to_read = msg_len;
    if (to_read > max_len) to_read = max_len;

    if (to_read > 0) {
        memcpy(data, client->recv_buffer + 3, to_read);
    }

    /* Remove message from buffer */
    {
        int consumed = 3 + msg_len;
        if (client->recv_len > consumed) {
            memmove(client->recv_buffer,
                    client->recv_buffer + consumed,
                    client->recv_len - consumed);
        }
        client->recv_len -= consumed;
    }

    client->messages_recv++;
    client->last_recv = get_time();

    return msg_len;
}

int ipc_client_request(IPCClient* client, u8 msg_type,
                       const u8* send_data, int send_len,
                       u8* resp_type, u8* resp_data, int max_len,
                       u32 timeout_ms)
{
    fd_set rfds;
    struct timeval tv;
    int ret;
    u32 start;
    u32 elapsed;

    if (!client || !resp_type || !resp_data) {
        return ENGINE_ERROR_PARAM;
    }

    /* Send request */
    ret = ipc_client_send(client, msg_type, send_data, send_len);
    if (ret != ENGINE_OK) {
        return ret;
    }

    /* Wait for response */
    start = get_time();

    while (1) {
        elapsed = (get_time() - start) * 1000;
        if (elapsed >= timeout_ms) {
            return ENGINE_ERROR_TIMEOUT;
        }

        FD_ZERO(&rfds);
        FD_SET(client->fd, &rfds);

        tv.tv_sec = (timeout_ms - elapsed) / 1000;
        tv.tv_usec = ((timeout_ms - elapsed) % 1000) * 1000;

        ret = select(client->fd + 1, &rfds, NULL, NULL, &tv);

        if (ret < 0) {
            if (errno == EINTR) continue;
            ipc_client_disconnect(client);
            return ENGINE_ERROR_IO;
        }

        if (ret == 0) {
            return ENGINE_ERROR_TIMEOUT;
        }

        /* Try to receive */
        ret = ipc_client_recv(client, resp_type, resp_data, max_len);

        if (ret < 0) {
            return ret;
        }

        if (ret > 0) {
            return ret;  /* Got response */
        }
    }
}

const char* ipc_service_name(IPCServiceType type)
{
    if (type >= 0 && type < IPC_SERVICE_MAX) {
        return service_names[type];
    }
    return "UNKNOWN";
}
