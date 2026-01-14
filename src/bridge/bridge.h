/*
 * bridge.h - DNF Bridge Service Header
 *
 * Bridge 是客户端连接的第一个入口点
 * 负责接受连接、解析协议头、路由到 Channel/Game
 */

#ifndef __BRIDGE_H__
#define __BRIDGE_H__

#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>

/* 版本信息 */
#define BRIDGE_VERSION_MAJOR  1
#define BRIDGE_VERSION_MINOR  0
#define BRIDGE_VERSION_PATCH  0

/* 默认配置 */
#define BRIDGE_DEFAULT_PORT       7000
#define BRIDGE_DEFAULT_MAX_CLIENT 1000
#define BRIDGE_DEFAULT_IP         "0.0.0.0"

/* 协议头大小 */
#define PACKET_HEADER_SIZE        13

/* 缓冲区大小 */
#define RECV_BUFFER_SIZE          65536
#define SEND_BUFFER_SIZE          65536
#define MAX_PACKET_SIZE           65536

/* 客户端状态 */
typedef enum {
    CLIENT_STATE_DISCONNECTED = 0,
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_CONNECTED,
    CLIENT_STATE_AUTHENTICATED,
    CLIENT_STATE_IN_CHANNEL,
    CLIENT_STATE_IN_GAME
} client_state_t;

/* 路由目标 */
typedef enum {
    ROUTE_TARGET_NONE = 0,
    ROUTE_TARGET_CHANNEL,
    ROUTE_TARGET_GAME,
    ROUTE_TARGET_BRIDGE_INTERNAL
} route_target_t;

/*
 * 协议头结构 (13字节)
 * 基于配置文件中的定义:
 *   header_classification = 0 1   (偏移0, 1字节)
 *   header_msg_no = 5 1           (偏移5, 1字节, 用低5位)
 *   header_sLength = 1 4          (偏移1, 4字节)
 *   header_checksum = 7 4         (偏移7, 4字节)
 *   header_sequence = 11 2        (偏移11, 2字节)
 *   header_mangled_length = 13
 */
#pragma pack(push, 1)
typedef struct packet_header {
    uint8_t  classification;    /* [0:1]  分类标识 */
    uint32_t length;            /* [1:4]  数据包长度 */
    uint8_t  msg_no;            /* [5:1]  消息号 (低5位有效) */
    uint8_t  reserved;          /* [6:1]  保留 */
    uint32_t checksum;          /* [7:4]  校验和 */
    uint16_t sequence;          /* [11:2] 序列号 */
} packet_header_t;
#pragma pack(pop)

/* 客户端连接结构 */
typedef struct client_conn {
    int                 fd;             /* 套接字描述符 */
    uint32_t            client_id;      /* 客户端ID */
    client_state_t      state;          /* 连接状态 */
    struct sockaddr_in  addr;           /* 客户端地址 */
    uint64_t            connect_time;   /* 连接时间 */
    uint64_t            last_activity;  /* 最后活动时间 */
    uint16_t            sequence;       /* 当前序列号 */
    uint8_t             recv_buffer[RECV_BUFFER_SIZE];
    int                 recv_len;       /* 接收缓冲区数据长度 */
    uint8_t             send_buffer[SEND_BUFFER_SIZE];
    int                 send_len;       /* 发送缓冲区数据长度 */
    pthread_mutex_t     lock;           /* 连接锁 */
} client_conn_t;

/* Bridge 配置 */
typedef struct bridge_config {
    char        listen_ip[64];      /* 监听IP */
    int         tcp_port;           /* TCP端口 */
    int         udp_port;           /* UDP端口 */
    int         max_clients;        /* 最大客户端数 */

    /* 数据库配置 */
    char        db_ip[64];          /* 数据库IP */
    char        db_name[64];        /* 数据库名 */
    char        db_user[64];        /* 数据库用户 */
    char        db_pass[64];        /* 数据库密码 */
    int         db_port;            /* 数据库端口 */

    /* Channel 服务配置 */
    char        channel_ip[64];     /* Channel IP */
    int         channel_port;       /* Channel 端口 */
} bridge_config_t;

/* Bridge 服务器结构 */
typedef struct bridge_server {
    int                 tcp_fd;         /* TCP 监听套接字 */
    int                 udp_fd;         /* UDP 套接字 */
    int                 epoll_fd;       /* epoll 描述符 */
    bridge_config_t     config;         /* 配置 */
    client_conn_t*      clients;        /* 客户端数组 */
    int                 client_count;   /* 当前客户端数 */
    volatile int        running;        /* 运行标志 */
    pthread_mutex_t     clients_lock;   /* 客户端数组锁 */

    /* 统计 */
    uint64_t            total_connections;
    uint64_t            total_packets;
    uint64_t            bytes_received;
    uint64_t            bytes_sent;
} bridge_server_t;

/* 函数声明 */

/* 初始化和清理 */
int bridge_init(bridge_server_t* server, const char* config_path);
void bridge_cleanup(bridge_server_t* server);

/* 配置解析 */
int bridge_load_config(const char* path, bridge_config_t* config);

/* 网络操作 */
int bridge_setup_tcp(bridge_server_t* server);
int bridge_setup_udp(bridge_server_t* server);
int bridge_setup_epoll(bridge_server_t* server);

/* 主循环 */
int bridge_run(bridge_server_t* server);
void bridge_stop(bridge_server_t* server);

/* 客户端管理 */
client_conn_t* bridge_accept_client(bridge_server_t* server);
void bridge_disconnect_client(bridge_server_t* server, client_conn_t* client);
client_conn_t* bridge_find_client(bridge_server_t* server, uint32_t client_id);

/* 协议处理 */
int bridge_parse_header(const uint8_t* data, int len, packet_header_t* header);
int bridge_validate_header(const packet_header_t* header);
uint32_t bridge_calc_checksum(const uint8_t* data, int len);

/* 路由 */
route_target_t bridge_determine_route(const packet_header_t* header);
int bridge_route_to_channel(bridge_server_t* server, client_conn_t* client,
                            const uint8_t* data, int len);
int bridge_route_to_game(bridge_server_t* server, client_conn_t* client,
                         const uint8_t* data, int len);

/* 数据接收发送 */
int bridge_recv_tcp(bridge_server_t* server, client_conn_t* client);
int bridge_send_tcp(client_conn_t* client, const uint8_t* data, int len);
int bridge_recv_udp(bridge_server_t* server);
int bridge_send_udp(bridge_server_t* server, struct sockaddr_in* addr,
                    const uint8_t* data, int len);

/* 日志 */
void bridge_log(int level, const char* fmt, ...);

/* 日志级别 */
#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARN    2
#define LOG_ERROR   3

#endif /* __BRIDGE_H__ */
