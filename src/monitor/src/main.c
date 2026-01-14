/*
 * main.c - DNF Monitor Service
 *
 * Central monitoring hub (UDP port 30303).
 * Collects heartbeats from all services and tracks health status.
 *
 * Compatible with GCC 4.1.2 / CentOS 5.8 (VM-131)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <pthread.h>

#include "monitor.h"

/*==============================================================================
 * Configuration
 *============================================================================*/

#define DEFAULT_PORT        30303
#define MAX_EVENTS          64
#define BUFFER_SIZE         4096
#define CHECK_INTERVAL      5       /* Check service health every 5 seconds */

/*==============================================================================
 * Global State
 *============================================================================*/

static volatile int g_running = 1;
static int g_udp_fd = -1;
static int g_tcp_fd = -1;
static int g_epoll_fd = -1;
static uint16_t g_port = DEFAULT_PORT;
static uint32_t g_next_service_id = 1;

/* Registered services */
static ServiceStatusEntry g_services[MONITOR_MAX_SERVICES];
static int g_service_count = 0;
static pthread_mutex_t g_service_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Matching queue (P2) */
static MatchQueueEntry g_match_queue[MATCH_MAX_QUEUE];
static int g_match_queue_count = 0;
static uint32_t g_next_match_id = 1;
static MatchRoom g_match_rooms[MATCH_MAX_ROOMS];
static int g_match_room_count = 0;
static uint32_t g_next_room_id = 1;
static pthread_mutex_t g_match_mutex = PTHREAD_MUTEX_INITIALIZER;

/*==============================================================================
 * Signal Handling
 *============================================================================*/

static void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        printf("[MONITOR] Received signal %d, shutting down...\n", signo);
        g_running = 0;
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);
}

/*==============================================================================
 * Utility Functions
 *============================================================================*/

static const char* service_type_name(uint8_t type) {
    switch (type) {
        case SERVICE_TYPE_BRIDGE:    return "BRIDGE";
        case SERVICE_TYPE_CHANNEL:   return "CHANNEL";
        case SERVICE_TYPE_GAME:      return "GAME";
        case SERVICE_TYPE_GUILD:     return "GUILD";
        case SERVICE_TYPE_AUCTION:   return "AUCTION";
        case SERVICE_TYPE_POINT:     return "POINT";
        case SERVICE_TYPE_RELAY:     return "RELAY";
        case SERVICE_TYPE_STUN:      return "STUN";
        case SERVICE_TYPE_DBMW:      return "DBMW";
        case SERVICE_TYPE_MANAGER:   return "MANAGER";
        case SERVICE_TYPE_COMMUNITY: return "COMMUNITY";
        case SERVICE_TYPE_STATICS:   return "STATICS";
        default:                     return "UNKNOWN";
    }
}

static const char* service_status_name(uint8_t status) {
    switch (status) {
        case SERVICE_STATUS_STARTING: return "STARTING";
        case SERVICE_STATUS_RUNNING:  return "RUNNING";
        case SERVICE_STATUS_STOPPING: return "STOPPING";
        case SERVICE_STATUS_STOPPED:  return "STOPPED";
        case SERVICE_STATUS_DEAD:     return "DEAD";
        case SERVICE_STATUS_ERROR:    return "ERROR";
        default:                      return "UNKNOWN";
    }
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*==============================================================================
 * Service Management
 *============================================================================*/

static ServiceStatusEntry* find_service(uint32_t service_id) {
    for (int i = 0; i < g_service_count; i++) {
        if (g_services[i].service_id == service_id) {
            return &g_services[i];
        }
    }
    return NULL;
}

static ServiceStatusEntry* find_service_by_addr(uint32_t ip, uint16_t port) {
    for (int i = 0; i < g_service_count; i++) {
        if (g_services[i].ip == ip && g_services[i].port == port) {
            return &g_services[i];
        }
    }
    return NULL;
}

static int register_service(const ServiceRegistration* reg, uint32_t ip) {
    pthread_mutex_lock(&g_service_mutex);

    /* Check if already registered */
    ServiceStatusEntry* existing = find_service_by_addr(ip, reg->port);
    if (existing) {
        /* Update existing entry */
        existing->pid = reg->pid;
        existing->status = SERVICE_STATUS_RUNNING;
        existing->start_time = reg->start_time;
        existing->last_heartbeat = (uint32_t)time(NULL);
        pthread_mutex_unlock(&g_service_mutex);
        printf("[MONITOR] Service re-registered: %s (id=%u)\n",
               existing->service_name, existing->service_id);
        return existing->service_id;
    }

    if (g_service_count >= MONITOR_MAX_SERVICES) {
        pthread_mutex_unlock(&g_service_mutex);
        printf("[MONITOR] Max services reached\n");
        return -1;
    }

    /* Add new service */
    ServiceStatusEntry* entry = &g_services[g_service_count];
    memset(entry, 0, sizeof(ServiceStatusEntry));

    entry->service_id = g_next_service_id++;
    entry->service_type = reg->service_type;
    strncpy(entry->service_name, reg->service_name, MONITOR_MAX_NAME_LEN);
    entry->ip = ip;
    entry->port = reg->port;
    entry->pid = reg->pid;
    entry->status = SERVICE_STATUS_RUNNING;
    entry->start_time = reg->start_time;
    entry->last_heartbeat = (uint32_t)time(NULL);

    g_service_count++;

    pthread_mutex_unlock(&g_service_mutex);

    printf("[MONITOR] Service registered: %s (%s) id=%u port=%u\n",
           entry->service_name, service_type_name(entry->service_type),
           entry->service_id, entry->port);

    return entry->service_id;
}

static void update_heartbeat(uint32_t service_id, const ServiceHeartbeat* hb) {
    pthread_mutex_lock(&g_service_mutex);

    ServiceStatusEntry* entry = find_service(service_id);
    if (entry) {
        entry->last_heartbeat = (uint32_t)time(NULL);
        entry->player_count = hb->player_count;
        entry->cpu_usage = hb->cpu_usage;
        entry->memory_usage = hb->memory_usage;
        entry->connections = hb->connections;
        entry->status = SERVICE_STATUS_RUNNING;
        entry->uptime = entry->last_heartbeat - entry->start_time;
    }

    pthread_mutex_unlock(&g_service_mutex);
}

static void check_service_health(void) {
    uint32_t now = (uint32_t)time(NULL);

    pthread_mutex_lock(&g_service_mutex);

    for (int i = 0; i < g_service_count; i++) {
        ServiceStatusEntry* entry = &g_services[i];

        if (entry->status == SERVICE_STATUS_RUNNING ||
            entry->status == SERVICE_STATUS_STARTING) {

            uint32_t elapsed = now - entry->last_heartbeat;

            if (elapsed > MONITOR_TIMEOUT) {
                printf("[MONITOR] Service DEAD: %s (id=%u) - no heartbeat for %u seconds\n",
                       entry->service_name, entry->service_id, elapsed);
                entry->status = SERVICE_STATUS_DEAD;
            }
        }
    }

    pthread_mutex_unlock(&g_service_mutex);
}

static void handle_shutdown(uint32_t service_id) {
    pthread_mutex_lock(&g_service_mutex);

    ServiceStatusEntry* entry = find_service(service_id);
    if (entry) {
        entry->status = SERVICE_STATUS_STOPPED;
        printf("[MONITOR] Service shutdown: %s (id=%u)\n",
               entry->service_name, entry->service_id);
    }

    pthread_mutex_unlock(&g_service_mutex);
}


/*==============================================================================
 * Matching Queue Management (P2)
 *============================================================================*/

int match_enqueue(const MatchEnqueueRequest* req, uint32_t ip) {
    pthread_mutex_lock(&g_match_mutex);

    if (g_match_queue_count >= MATCH_MAX_QUEUE) {
        pthread_mutex_unlock(&g_match_mutex);
        printf("[MONITOR] Match queue full
");
        return -1;
    }

    MatchQueueEntry* entry = &g_match_queue[g_match_queue_count];
    memset(entry, 0, sizeof(MatchQueueEntry));

    entry->match_id = g_next_match_id++;
    entry->charac_no = req->charac_no;
    entry->account_id = req->account_id;
    strncpy(entry->charac_name, req->charac_name, 20);
    entry->level = req->level;
    entry->job = req->job;
    entry->dungeon_type = req->dungeon_type;
    entry->dungeon_id = req->dungeon_id;
    entry->channel_no = req->channel_no;
    entry->game_server_ip = ip;
    entry->game_server_port = req->game_server_port;
    entry->enqueue_time = (uint32_t)time(NULL);
    entry->status = MATCH_STATUS_QUEUED;
    entry->room_id = 0;

    g_match_queue_count++;

    pthread_mutex_unlock(&g_match_mutex);

    printf("[MONITOR] Match enqueue: %s (id=%u) dungeon=%u
",
           entry->charac_name, entry->match_id, entry->dungeon_id);

    return entry->match_id;
}

int match_cancel(uint32_t match_id) {
    pthread_mutex_lock(&g_match_mutex);

    for (int i = 0; i < g_match_queue_count; i++) {
        if (g_match_queue[i].match_id == match_id) {
            printf("[MONITOR] Match cancel: %s (id=%u)
",
                   g_match_queue[i].charac_name, match_id);

            /* Remove by shifting */
            for (int j = i; j < g_match_queue_count - 1; j++) {
                g_match_queue[j] = g_match_queue[j + 1];
            }
            g_match_queue_count--;

            pthread_mutex_unlock(&g_match_mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&g_match_mutex);
    return -1;
}

static MatchQueueEntry* match_find_by_id(uint32_t match_id) {
    for (int i = 0; i < g_match_queue_count; i++) {
        if (g_match_queue[i].match_id == match_id) {
            return &g_match_queue[i];
        }
    }
    return NULL;
}

int match_process_queue(void) {
    pthread_mutex_lock(&g_match_mutex);

    /* Simple matching: group players by dungeon_id */
    for (int i = 0; i < g_match_queue_count; i++) {
        if (g_match_queue[i].status != MATCH_STATUS_QUEUED) continue;

        uint32_t dungeon_id = g_match_queue[i].dungeon_id;
        int candidates[4];
        int candidate_count = 0;

        /* Find up to 4 players for same dungeon */
        for (int j = i; j < g_match_queue_count && candidate_count < 4; j++) {
            if (g_match_queue[j].status == MATCH_STATUS_QUEUED &&
                g_match_queue[j].dungeon_id == dungeon_id) {
                candidates[candidate_count++] = j;
            }
        }

        /* Need at least 2 players to form a match */
        if (candidate_count >= 2) {
            /* Create match room */
            if (g_match_room_count >= MATCH_MAX_ROOMS) continue;

            MatchRoom* room = &g_match_rooms[g_match_room_count];
            memset(room, 0, sizeof(MatchRoom));

            room->room_id = g_next_room_id++;
            room->dungeon_id = dungeon_id;
            room->member_count = candidate_count;
            room->status = 0;
            room->create_time = (uint32_t)time(NULL);

            for (int k = 0; k < candidate_count; k++) {
                int idx = candidates[k];
                room->members[k] = g_match_queue[idx].match_id;
                g_match_queue[idx].status = MATCH_STATUS_MATCHED;
                g_match_queue[idx].room_id = room->room_id;
            }

            g_match_room_count++;

            printf("[MONITOR] Match room created: room=%u dungeon=%u players=%d
",
                   room->room_id, dungeon_id, candidate_count);
        }
    }

    pthread_mutex_unlock(&g_match_mutex);
}

/*==============================================================================
 * Network Functions
 *============================================================================*/

static int init_server(void) {
    /* UDP socket for heartbeats */
    g_udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_udp_fd < 0) {
        printf("[MONITOR] Failed to create UDP socket: %s\n", strerror(errno));
        return -1;
    }

    int opt = 1;
    setsockopt(g_udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_port);

    if (bind(g_udp_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[MONITOR] Failed to bind UDP port %d: %s\n", g_port, strerror(errno));
        close(g_udp_fd);
        return -1;
    }

    set_nonblocking(g_udp_fd);

    /* TCP socket for commands */
    g_tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_tcp_fd < 0) {
        printf("[MONITOR] Failed to create TCP socket: %s\n", strerror(errno));
        close(g_udp_fd);
        return -1;
    }

    setsockopt(g_tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_port = htons(g_port + 1);  /* TCP on port+1 */

    if (bind(g_tcp_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[MONITOR] Failed to bind TCP port %d: %s\n", g_port + 1, strerror(errno));
        close(g_udp_fd);
        close(g_tcp_fd);
        return -1;
    }

    if (listen(g_tcp_fd, 32) < 0) {
        printf("[MONITOR] Failed to listen: %s\n", strerror(errno));
        close(g_udp_fd);
        close(g_tcp_fd);
        return -1;
    }

    set_nonblocking(g_tcp_fd);

    /* Epoll */
    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        printf("[MONITOR] Failed to create epoll: %s\n", strerror(errno));
        close(g_udp_fd);
        close(g_tcp_fd);
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_udp_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_udp_fd, &ev);

    ev.data.fd = g_tcp_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_tcp_fd, &ev);

    printf("[MONITOR] Server listening on UDP:%d, TCP:%d\n", g_port, g_port + 1);
    return 0;
}

static void cleanup_server(void) {
    if (g_epoll_fd >= 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }
    if (g_udp_fd >= 0) {
        close(g_udp_fd);
        g_udp_fd = -1;
    }
    if (g_tcp_fd >= 0) {
        close(g_tcp_fd);
        g_tcp_fd = -1;
    }
}

/*==============================================================================
 * Message Processing
 *============================================================================*/

static void send_udp_response(struct sockaddr_in* addr, uint8_t msg_type,
                              uint32_t result, const uint8_t* data, int len) {
    uint8_t buffer[BUFFER_SIZE];
    int offset = 0;

    buffer[offset++] = msg_type;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    if (data && len > 0) {
        memcpy(buffer + offset, data, len);
        offset += len;
    }

    sendto(g_udp_fd, buffer, offset, 0, (struct sockaddr*)addr, sizeof(*addr));
}

static void process_udp_message(const uint8_t* data, int len, struct sockaddr_in* addr) {
    if (len < 1) return;

    uint8_t msg_type = data[0];
    const uint8_t* payload = data + 1;
    int payload_len = len - 1;

    switch (msg_type) {
        case MMSG_REGISTER: {
            if (payload_len < (int)sizeof(ServiceRegistration)) break;

            ServiceRegistration reg;
            memcpy(&reg, payload, sizeof(ServiceRegistration));

            int service_id = register_service(&reg, addr->sin_addr.s_addr);

            uint8_t response[4];
            memcpy(response, &service_id, 4);
            send_udp_response(addr, msg_type, 0, response, 4);
            break;
        }

        case MMSG_HEARTBEAT: {
            if (payload_len < (int)sizeof(ServiceHeartbeat)) break;

            ServiceHeartbeat hb;
            memcpy(&hb, payload, sizeof(ServiceHeartbeat));

            update_heartbeat(hb.service_id, &hb);
            send_udp_response(addr, msg_type, 0, NULL, 0);
            break;
        }

        case MMSG_SHUTDOWN: {
            if (payload_len < 4) break;

            uint32_t service_id;
            memcpy(&service_id, payload, 4);

            handle_shutdown(service_id);
            send_udp_response(addr, msg_type, 0, NULL, 0);
            break;
        }

        case MMSG_ALERT: {
            if (payload_len < 4) break;

            uint32_t service_id;
            memcpy(&service_id, payload, 4);
            const char* message = (const char*)(payload + 4);

            pthread_mutex_lock(&g_service_mutex);
            ServiceStatusEntry* entry = find_service(service_id);
            if (entry) {
                printf("[MONITOR] ALERT from %s: %s\n", entry->service_name, message);
            }
            pthread_mutex_unlock(&g_service_mutex);

            send_udp_response(addr, msg_type, 0, NULL, 0);
            break;
        }

        case MMSG_QUERY_STATUS: {
            if (payload_len < 4) break;

            uint32_t service_id;
            memcpy(&service_id, payload, 4);

            pthread_mutex_lock(&g_service_mutex);
            ServiceStatusEntry* entry = find_service(service_id);
            if (entry) {
                send_udp_response(addr, msg_type, 0, (uint8_t*)entry, sizeof(ServiceStatusEntry));
            } else {
                send_udp_response(addr, msg_type, 1, NULL, 0);
            }
            pthread_mutex_unlock(&g_service_mutex);
            break;
        }

        case MMSG_QUERY_ALL: {
            pthread_mutex_lock(&g_service_mutex);

            uint8_t response[4096];
            int offset = 0;

            uint8_t count = (uint8_t)g_service_count;
            response[offset++] = count;

            for (int i = 0; i < g_service_count && offset < 4000; i++) {
                /* Send abbreviated info */
                memcpy(response + offset, &g_services[i].service_id, 4); offset += 4;
                response[offset++] = g_services[i].service_type;
                response[offset++] = g_services[i].status;
                memcpy(response + offset, &g_services[i].player_count, 4); offset += 4;
                response[offset++] = g_services[i].cpu_usage;
                response[offset++] = g_services[i].memory_usage;
            }

            pthread_mutex_unlock(&g_service_mutex);
            send_udp_response(addr, msg_type, 0, response, offset);
            break;
        }


        /* Matching system messages (P2) */
        case MMSG_MATCH_ENQUEUE: {
            if (payload_len < (int)sizeof(MatchEnqueueRequest)) break;

            MatchEnqueueRequest req;
            memcpy(&req, payload, sizeof(MatchEnqueueRequest));

            int match_id = match_enqueue(&req, addr->sin_addr.s_addr);

            uint8_t response[4];
            memcpy(response, &match_id, 4);
            send_udp_response(addr, msg_type, (match_id > 0) ? 0 : 1, response, 4);
            break;
        }

        case MMSG_MATCH_CANCEL: {
            if (payload_len < 4) break;

            uint32_t match_id;
            memcpy(&match_id, payload, 4);

            int result = match_cancel(match_id);
            send_udp_response(addr, msg_type, (result == 0) ? 0 : 1, NULL, 0);
            break;
        }

        case MMSG_MATCH_STATUS: {
            if (payload_len < 4) break;

            uint32_t match_id;
            memcpy(&match_id, payload, 4);

            pthread_mutex_lock(&g_match_mutex);
            MatchQueueEntry* entry = match_find_by_id(match_id);
            if (entry) {
                send_udp_response(addr, msg_type, 0, (uint8_t*)entry, sizeof(MatchQueueEntry));
            } else {
                send_udp_response(addr, msg_type, 1, NULL, 0);
            }
            pthread_mutex_unlock(&g_match_mutex);
            break;
        }

        case MMSG_MATCH_ACCEPT: {
            if (payload_len < 4) break;

            uint32_t match_id;
            memcpy(&match_id, payload, 4);

            pthread_mutex_lock(&g_match_mutex);
            MatchQueueEntry* entry = match_find_by_id(match_id);
            if (entry && entry->status == MATCH_STATUS_MATCHED) {
                entry->status = MATCH_STATUS_ACCEPTED;
                printf("[MONITOR] Match accepted: %s (id=%u)
", entry->charac_name, match_id);
                send_udp_response(addr, msg_type, 0, NULL, 0);
            } else {
                send_udp_response(addr, msg_type, 1, NULL, 0);
            }
            pthread_mutex_unlock(&g_match_mutex);
            break;
        }

        case MMSG_MATCH_DECLINE: {
            if (payload_len < 4) break;

            uint32_t match_id;
            memcpy(&match_id, payload, 4);

            pthread_mutex_lock(&g_match_mutex);
            MatchQueueEntry* entry = match_find_by_id(match_id);
            if (entry) {
                printf("[MONITOR] Match declined: %s (id=%u)
", entry->charac_name, match_id);
                entry->status = MATCH_STATUS_QUEUED;
                entry->room_id = 0;
                send_udp_response(addr, msg_type, 0, NULL, 0);
            } else {
                send_udp_response(addr, msg_type, 1, NULL, 0);
            }
            pthread_mutex_unlock(&g_match_mutex);
            break;
        }

        default:
            printf("[MONITOR] Unknown message type: %d\n", msg_type);
            send_udp_response(addr, msg_type, 1, NULL, 0);
            break;
    }
}

static void handle_udp(void) {
    uint8_t buffer[BUFFER_SIZE];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    ssize_t n = recvfrom(g_udp_fd, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)&addr, &addr_len);
    if (n <= 0) return;

    process_udp_message(buffer, (int)n, &addr);
}

static void handle_tcp_connection(int client_fd, struct sockaddr_in* addr) {
    uint8_t buffer[BUFFER_SIZE];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

    if (n <= 0) {
        close(client_fd);
        return;
    }

    /* Process TCP command - same format as UDP */
    if (n < 1) {
        close(client_fd);
        return;
    }

    uint8_t msg_type = buffer[0];

    switch (msg_type) {
        case MMSG_QUERY_ALL: {
            pthread_mutex_lock(&g_service_mutex);

            uint8_t response[4096];
            int offset = 0;

            response[offset++] = msg_type;
            response[offset++] = (uint8_t)g_service_count;

            for (int i = 0; i < g_service_count && offset < 3900; i++) {
                memcpy(response + offset, &g_services[i], sizeof(ServiceStatusEntry));
                offset += sizeof(ServiceStatusEntry);
            }

            pthread_mutex_unlock(&g_service_mutex);
            send(client_fd, response, offset, 0);
            break;
        }

        default:
            /* Forward to UDP handler logic */
            process_udp_message(buffer, (int)n, addr);
            break;
    }

    close(client_fd);
}

static void accept_tcp_connection(void) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(g_tcp_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("[MONITOR] Accept failed: %s\n", strerror(errno));
        }
        return;
    }

    handle_tcp_connection(client_fd, &client_addr);
}

/*==============================================================================
 * Status Display
 *============================================================================*/

static void print_status(void) {
    printf("\n========================================\n");
    printf("     DNF Monitor Service Status\n");
    printf("========================================\n");

    pthread_mutex_lock(&g_service_mutex);

    printf("Registered Services: %d\n\n", g_service_count);

    if (g_service_count > 0) {
        printf("%-4s %-12s %-10s %-8s %-5s %-5s %-8s\n",
               "ID", "Name", "Type", "Status", "CPU%", "MEM%", "Players");
        printf("------------------------------------------------------------\n");

        for (int i = 0; i < g_service_count; i++) {
            ServiceStatusEntry* e = &g_services[i];
            printf("%-4u %-12s %-10s %-8s %-5u %-5u %-8u\n",
                   e->service_id,
                   e->service_name,
                   service_type_name(e->service_type),
                   service_status_name(e->status),
                   e->cpu_usage,
                   e->memory_usage,
                   e->player_count);
        }
    }

    pthread_mutex_unlock(&g_service_mutex);
    printf("\n");
}

/*==============================================================================
 * API Implementation
 *============================================================================*/

int monitor_service_init(uint16_t port) {
    g_port = port;
    memset(g_services, 0, sizeof(g_services));
    return 0;
}

void monitor_service_cleanup(void) {
    cleanup_server();
}

int monitor_service_run(void) {
    struct epoll_event events[MAX_EVENTS];
    uint32_t last_check = 0;
    uint32_t last_status = 0;

    printf("[MONITOR] Monitor service running...\n");

    while (g_running) {
        int nfds = epoll_wait(g_epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            printf("[MONITOR] epoll_wait error: %s\n", strerror(errno));
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == g_udp_fd) {
                handle_udp();
            } else if (events[i].data.fd == g_tcp_fd) {
                accept_tcp_connection();
            }
        }

        /* Periodic health check */
        uint32_t now = (uint32_t)time(NULL);
        if (now - last_check >= CHECK_INTERVAL) {
            check_service_health();
            last_check = now;
        }

        /* Periodic status print */
        if (now - last_status >= 60) {
            print_status();
            last_status = now;
        }
    }

    return 0;
}

int monitor_get_service(uint32_t service_id, ServiceStatusEntry* entry) {
    if (!entry) return -1;

    pthread_mutex_lock(&g_service_mutex);
    ServiceStatusEntry* found = find_service(service_id);
    if (found) {
        memcpy(entry, found, sizeof(ServiceStatusEntry));
        pthread_mutex_unlock(&g_service_mutex);
        return 0;
    }
    pthread_mutex_unlock(&g_service_mutex);
    return -1;
}

int monitor_get_all_services(ServiceStatusEntry* entries, int max_count) {
    if (!entries) return -1;

    pthread_mutex_lock(&g_service_mutex);
    int count = (g_service_count < max_count) ? g_service_count : max_count;
    memcpy(entries, g_services, count * sizeof(ServiceStatusEntry));
    pthread_mutex_unlock(&g_service_mutex);

    return count;
}

int monitor_get_services_by_type(uint8_t type, ServiceStatusEntry* entries, int max_count) {
    if (!entries) return -1;

    pthread_mutex_lock(&g_service_mutex);
    int count = 0;
    for (int i = 0; i < g_service_count && count < max_count; i++) {
        if (g_services[i].service_type == type) {
            memcpy(&entries[count], &g_services[i], sizeof(ServiceStatusEntry));
            count++;
        }
    }
    pthread_mutex_unlock(&g_service_mutex);

    return count;
}

/*==============================================================================
 * Main Entry Point
 *============================================================================*/

static void print_usage(const char* prog) {
    printf("Usage: %s [port]\n", prog);
    printf("  port: Server port (default: %d)\n", DEFAULT_PORT);
}

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("     DNF Monitor Service v1.0\n");
    printf("========================================\n");

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        g_port = (uint16_t)atoi(argv[1]);
    }

    setup_signals();

    if (monitor_service_init(g_port) != 0) {
        printf("[MONITOR] Failed to initialize service\n");
        return 1;
    }

    if (init_server() != 0) {
        printf("[MONITOR] Failed to initialize server\n");
        monitor_service_cleanup();
        return 1;
    }

    int result = monitor_service_run();

    monitor_service_cleanup();
    printf("[MONITOR] Service stopped\n");

    return result;
}
