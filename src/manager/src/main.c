/*
 * main.c - DNF Manager Service
 *
 * Process lifecycle management (TCP port 40403).
 * Starts, stops, monitors, and auto-restarts game service processes.
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
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <pthread.h>

#include "manager.h"

/*==============================================================================
 * Configuration
 *============================================================================*/

#define DEFAULT_PORT        40403
#define MAX_EVENTS          64
#define BUFFER_SIZE         4096
#define CHECK_INTERVAL      5       /* Check processes every 5 seconds */

/*==============================================================================
 * Global State
 *============================================================================*/

static volatile int g_running = 1;
static int g_listen_fd = -1;
static int g_epoll_fd = -1;
static uint16_t g_port = DEFAULT_PORT;
static uint32_t g_next_proc_id = 1;

/* Process configurations and runtime info */
static ProcessConfig g_configs[MANAGER_MAX_PROCESSES];
static ProcessInfo g_processes[MANAGER_MAX_PROCESSES];
static int g_process_count = 0;
static pthread_mutex_t g_proc_mutex = PTHREAD_MUTEX_INITIALIZER;

/*==============================================================================
 * Signal Handling
 *============================================================================*/

static volatile int g_child_exited = 0;

static void signal_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        printf("[MANAGER] Received signal %d, shutting down...\n", signo);
        g_running = 0;
    } else if (signo == SIGCHLD) {
        g_child_exited = 1;
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);
}

/*==============================================================================
 * Utility Functions
 *============================================================================*/

static const char* proc_type_name(uint8_t type) {
    switch (type) {
        case PROC_TYPE_BRIDGE:    return "BRIDGE";
        case PROC_TYPE_CHANNEL:   return "CHANNEL";
        case PROC_TYPE_GAME:      return "GAME";
        case PROC_TYPE_GUILD:     return "GUILD";
        case PROC_TYPE_AUCTION:   return "AUCTION";
        case PROC_TYPE_POINT:     return "POINT";
        case PROC_TYPE_RELAY:     return "RELAY";
        case PROC_TYPE_STUN:      return "STUN";
        case PROC_TYPE_DBMW:      return "DBMW";
        case PROC_TYPE_MONITOR:   return "MONITOR";
        case PROC_TYPE_COMMUNITY: return "COMMUNITY";
        case PROC_TYPE_STATICS:   return "STATICS";
        default:                  return "UNKNOWN";
    }
}

static const char* proc_status_name(uint8_t status) {
    switch (status) {
        case PROC_STATUS_STOPPED:  return "STOPPED";
        case PROC_STATUS_STARTING: return "STARTING";
        case PROC_STATUS_RUNNING:  return "RUNNING";
        case PROC_STATUS_STOPPING: return "STOPPING";
        case PROC_STATUS_CRASHED:  return "CRASHED";
        case PROC_STATUS_DISABLED: return "DISABLED";
        default:                   return "UNKNOWN";
    }
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/*==============================================================================
 * Process Management
 *============================================================================*/

static ProcessInfo* find_process(const char* name) {
    for (int i = 0; i < g_process_count; i++) {
        if (strcmp(g_processes[i].name, name) == 0) {
            return &g_processes[i];
        }
    }
    return NULL;
}

static ProcessConfig* find_config(const char* name) {
    for (int i = 0; i < g_process_count; i++) {
        if (strcmp(g_configs[i].name, name) == 0) {
            return &g_configs[i];
        }
    }
    return NULL;
}

static int start_process(const char* name) {
    pthread_mutex_lock(&g_proc_mutex);

    ProcessInfo* proc = find_process(name);
    ProcessConfig* config = find_config(name);

    if (!proc || !config) {
        pthread_mutex_unlock(&g_proc_mutex);
        printf("[MANAGER] Process not found: %s\n", name);
        return -1;
    }

    if (!config->enabled) {
        pthread_mutex_unlock(&g_proc_mutex);
        printf("[MANAGER] Process disabled: %s\n", name);
        return -2;
    }

    if (proc->status == PROC_STATUS_RUNNING) {
        pthread_mutex_unlock(&g_proc_mutex);
        printf("[MANAGER] Process already running: %s (pid=%d)\n", name, proc->pid);
        return 0;
    }

    proc->status = PROC_STATUS_STARTING;
    pthread_mutex_unlock(&g_proc_mutex);

    /* Fork and exec */
    pid_t pid = fork();
    if (pid < 0) {
        printf("[MANAGER] Fork failed: %s\n", strerror(errno));
        pthread_mutex_lock(&g_proc_mutex);
        proc->status = PROC_STATUS_CRASHED;
        pthread_mutex_unlock(&g_proc_mutex);
        return -3;
    }

    if (pid == 0) {
        /* Child process */
        if (strlen(config->work_dir) > 0) {
            chdir(config->work_dir);
        }

        /* Close file descriptors */
        for (int fd = 3; fd < 256; fd++) {
            close(fd);
        }

        /* Redirect stdout/stderr to log file */
        char log_file[512];
        snprintf(log_file, sizeof(log_file), "/tmp/%s.log", name);
        int log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (log_fd >= 0) {
            dup2(log_fd, STDOUT_FILENO);
            dup2(log_fd, STDERR_FILENO);
            close(log_fd);
        }

        /* Parse args and exec */
        char* argv[64];
        int argc = 0;

        argv[argc++] = config->path;

        /* Simple arg parsing (space-separated) */
        char args_copy[MANAGER_MAX_ARGS_LEN + 1];
        strncpy(args_copy, config->args, MANAGER_MAX_ARGS_LEN);
        args_copy[MANAGER_MAX_ARGS_LEN] = '\0';

        char* token = strtok(args_copy, " ");
        while (token && argc < 63) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        execv(config->path, argv);

        /* If we get here, exec failed */
        printf("exec failed: %s - %s\n", config->path, strerror(errno));
        _exit(127);
    }

    /* Parent process */
    pthread_mutex_lock(&g_proc_mutex);
    proc->pid = pid;
    proc->status = PROC_STATUS_RUNNING;
    proc->start_time = (uint32_t)time(NULL);
    proc->restart_count++;
    proc->last_restart = proc->start_time;
    pthread_mutex_unlock(&g_proc_mutex);

    printf("[MANAGER] Started process: %s (pid=%d)\n", name, pid);
    return 0;
}

static int stop_process(const char* name, int force) {
    pthread_mutex_lock(&g_proc_mutex);

    ProcessInfo* proc = find_process(name);
    if (!proc) {
        pthread_mutex_unlock(&g_proc_mutex);
        return -1;
    }

    if (proc->status != PROC_STATUS_RUNNING) {
        pthread_mutex_unlock(&g_proc_mutex);
        return 0;
    }

    pid_t pid = proc->pid;
    proc->status = PROC_STATUS_STOPPING;
    pthread_mutex_unlock(&g_proc_mutex);

    /* Send signal */
    if (force) {
        kill(pid, SIGKILL);
    } else {
        kill(pid, SIGTERM);
    }

    printf("[MANAGER] Stopping process: %s (pid=%d, force=%d)\n", name, pid, force);
    return 0;
}

static void check_child_processes(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        pthread_mutex_lock(&g_proc_mutex);

        /* Find which process exited */
        for (int i = 0; i < g_process_count; i++) {
            if (g_processes[i].pid == pid) {
                g_processes[i].stop_time = (uint32_t)time(NULL);

                if (WIFEXITED(status)) {
                    g_processes[i].exit_code = WEXITSTATUS(status);
                    printf("[MANAGER] Process exited: %s (pid=%d, code=%d)\n",
                           g_processes[i].name, pid, g_processes[i].exit_code);
                } else if (WIFSIGNALED(status)) {
                    g_processes[i].exit_code = -WTERMSIG(status);
                    printf("[MANAGER] Process killed: %s (pid=%d, signal=%d)\n",
                           g_processes[i].name, pid, WTERMSIG(status));
                }

                if (g_processes[i].status == PROC_STATUS_STOPPING) {
                    g_processes[i].status = PROC_STATUS_STOPPED;
                } else {
                    g_processes[i].status = PROC_STATUS_CRASHED;
                }

                g_processes[i].pid = 0;
                break;
            }
        }

        pthread_mutex_unlock(&g_proc_mutex);
    }
}

static void check_auto_restart(void) {
    uint32_t now = (uint32_t)time(NULL);

    pthread_mutex_lock(&g_proc_mutex);

    for (int i = 0; i < g_process_count; i++) {
        ProcessInfo* proc = &g_processes[i];
        ProcessConfig* config = &g_configs[i];

        if (proc->status == PROC_STATUS_CRASHED && config->auto_restart && config->enabled) {
            /* Check restart delay */
            if (now - proc->stop_time >= config->restart_delay) {
                /* Check restart limit */
                uint32_t hour_ago = now - 3600;
                if (config->max_restarts == 0 ||
                    proc->restart_count < config->max_restarts ||
                    proc->last_restart < hour_ago) {

                    printf("[MANAGER] Auto-restarting: %s\n", proc->name);
                    pthread_mutex_unlock(&g_proc_mutex);
                    start_process(proc->name);
                    pthread_mutex_lock(&g_proc_mutex);
                } else {
                    printf("[MANAGER] Max restarts reached for: %s\n", proc->name);
                    proc->status = PROC_STATUS_DISABLED;
                    config->enabled = 0;
                }
            }
        }
    }

    pthread_mutex_unlock(&g_proc_mutex);
}

/*==============================================================================
 * Network Functions
 *============================================================================*/

static int init_server(void) {
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        printf("[MANAGER] Failed to create socket: %s\n", strerror(errno));
        return -1;
    }

    int opt = 1;
    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(g_port);

    if (bind(g_listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[MANAGER] Failed to bind port %d: %s\n", g_port, strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    if (listen(g_listen_fd, 32) < 0) {
        printf("[MANAGER] Failed to listen: %s\n", strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    set_nonblocking(g_listen_fd);

    g_epoll_fd = epoll_create(MAX_EVENTS);
    if (g_epoll_fd < 0) {
        printf("[MANAGER] Failed to create epoll: %s\n", strerror(errno));
        close(g_listen_fd);
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = g_listen_fd;
    epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_listen_fd, &ev);

    printf("[MANAGER] Server listening on port %d\n", g_port);
    return 0;
}

static void cleanup_server(void) {
    if (g_epoll_fd >= 0) {
        close(g_epoll_fd);
        g_epoll_fd = -1;
    }
    if (g_listen_fd >= 0) {
        close(g_listen_fd);
        g_listen_fd = -1;
    }
}

/*==============================================================================
 * Message Processing
 *============================================================================*/

static void send_response(int fd, uint8_t msg_type, int32_t result,
                          const uint8_t* data, int len) {
    uint8_t buffer[BUFFER_SIZE];
    int offset = 0;

    buffer[offset++] = msg_type;
    memcpy(buffer + offset, &result, 4);
    offset += 4;

    if (data && len > 0) {
        memcpy(buffer + offset, data, len);
        offset += len;
    }

    send(fd, buffer, offset, 0);
}

static void process_message(int fd, const uint8_t* data, int len) {
    if (len < 1) return;

    uint8_t msg_type = data[0];
    const uint8_t* payload = data + 1;
    int payload_len = len - 1;

    switch (msg_type) {
        case MGMT_START_SERVICE: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            int result = start_process(name);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case MGMT_STOP_SERVICE: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            int result = stop_process(name, 0);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case MGMT_RESTART_SERVICE: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            stop_process(name, 0);
            sleep(1);
            int result = start_process(name);
            send_response(fd, msg_type, result, NULL, 0);
            break;
        }

        case MGMT_GET_STATUS: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            pthread_mutex_lock(&g_proc_mutex);
            ProcessInfo* proc = find_process(name);
            if (proc) {
                send_response(fd, msg_type, 0, (uint8_t*)proc, sizeof(ProcessInfo));
            } else {
                send_response(fd, msg_type, -1, NULL, 0);
            }
            pthread_mutex_unlock(&g_proc_mutex);
            break;
        }

        case MGMT_GET_ALL_STATUS: {
            pthread_mutex_lock(&g_proc_mutex);

            uint8_t response[4096];
            int offset = 0;

            uint8_t count = (uint8_t)g_process_count;
            response[offset++] = count;

            for (int i = 0; i < g_process_count && offset < 3900; i++) {
                memcpy(response + offset, &g_processes[i], sizeof(ProcessInfo));
                offset += sizeof(ProcessInfo);
            }

            pthread_mutex_unlock(&g_proc_mutex);
            send_response(fd, msg_type, 0, response, offset);
            break;
        }

        case MGMT_KILL_PROCESS: {
            if (payload_len < 4) break;
            pid_t pid;
            memcpy(&pid, payload, 4);

            int result = kill(pid, SIGKILL);
            send_response(fd, msg_type, result, NULL, 0);
            printf("[MANAGER] Kill process: pid=%d result=%d\n", pid, result);
            break;
        }

        case MGMT_ADD_SERVICE: {
            if (payload_len < (int)sizeof(ProcessConfig)) break;

            ProcessConfig config;
            memcpy(&config, payload, sizeof(ProcessConfig));

            pthread_mutex_lock(&g_proc_mutex);
            if (g_process_count >= MANAGER_MAX_PROCESSES) {
                pthread_mutex_unlock(&g_proc_mutex);
                send_response(fd, msg_type, -1, NULL, 0);
                break;
            }

            /* Check if exists */
            if (find_config(config.name)) {
                pthread_mutex_unlock(&g_proc_mutex);
                send_response(fd, msg_type, -2, NULL, 0);
                break;
            }

            memcpy(&g_configs[g_process_count], &config, sizeof(ProcessConfig));

            memset(&g_processes[g_process_count], 0, sizeof(ProcessInfo));
            g_processes[g_process_count].proc_id = g_next_proc_id++;
            strncpy(g_processes[g_process_count].name, config.name, MANAGER_MAX_NAME_LEN);
            g_processes[g_process_count].type = config.type;
            g_processes[g_process_count].status = PROC_STATUS_STOPPED;

            g_process_count++;
            pthread_mutex_unlock(&g_proc_mutex);

            printf("[MANAGER] Added service: %s\n", config.name);
            send_response(fd, msg_type, 0, NULL, 0);
            break;
        }

        case MGMT_ENABLE_SERVICE: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            pthread_mutex_lock(&g_proc_mutex);
            ProcessConfig* config = find_config(name);
            ProcessInfo* proc = find_process(name);
            if (config && proc) {
                config->enabled = 1;
                if (proc->status == PROC_STATUS_DISABLED) {
                    proc->status = PROC_STATUS_STOPPED;
                }
                send_response(fd, msg_type, 0, NULL, 0);
            } else {
                send_response(fd, msg_type, -1, NULL, 0);
            }
            pthread_mutex_unlock(&g_proc_mutex);
            break;
        }

        case MGMT_DISABLE_SERVICE: {
            if (payload_len < 1) break;
            char name[MANAGER_MAX_NAME_LEN + 1];
            int name_len = (payload_len < MANAGER_MAX_NAME_LEN) ? payload_len : MANAGER_MAX_NAME_LEN;
            memcpy(name, payload, name_len);
            name[name_len] = '\0';

            pthread_mutex_lock(&g_proc_mutex);
            ProcessConfig* config = find_config(name);
            if (config) {
                config->enabled = 0;
                send_response(fd, msg_type, 0, NULL, 0);
            } else {
                send_response(fd, msg_type, -1, NULL, 0);
            }
            pthread_mutex_unlock(&g_proc_mutex);
            break;
        }

        default:
            printf("[MANAGER] Unknown message type: %d\n", msg_type);
            send_response(fd, msg_type, -1, NULL, 0);
            break;
    }
}

static void handle_client(int fd) {
    uint8_t buffer[BUFFER_SIZE];
    ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

    if (n <= 0) {
        close(fd);
        return;
    }

    process_message(fd, buffer, (int)n);
    close(fd);
}

static void accept_connection(void) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(g_listen_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            printf("[MANAGER] Accept failed: %s\n", strerror(errno));
        }
        return;
    }

    handle_client(client_fd);
}

/*==============================================================================
 * Status Display
 *============================================================================*/

static void print_status(void) {
    printf("\n========================================\n");
    printf("     DNF Manager Service Status\n");
    printf("========================================\n");

    pthread_mutex_lock(&g_proc_mutex);

    printf("Managed Processes: %d\n\n", g_process_count);

    if (g_process_count > 0) {
        printf("%-4s %-16s %-10s %-10s %-8s\n",
               "ID", "Name", "Type", "Status", "PID");
        printf("--------------------------------------------------------\n");

        for (int i = 0; i < g_process_count; i++) {
            ProcessInfo* p = &g_processes[i];
            printf("%-4u %-16s %-10s %-10s %-8d\n",
                   p->proc_id,
                   p->name,
                   proc_type_name(p->type),
                   proc_status_name(p->status),
                   p->pid);
        }
    }

    pthread_mutex_unlock(&g_proc_mutex);
    printf("\n");
}

/*==============================================================================
 * Default Configuration
 *============================================================================*/

static void load_default_config(void) {
    /* Add default game services */
    ProcessConfig configs[] = {
        { "bridge",     PROC_TYPE_BRIDGE,  "/home/neople/bridge/df_bridge_r",    "", "/home/neople/bridge",  1, 1, 5, 5 },
        { "channel",    PROC_TYPE_CHANNEL, "/home/neople/channel/df_channel_r",  "", "/home/neople/channel", 1, 1, 5, 5 },
        { "game_cain01",PROC_TYPE_GAME,    "/home/neople/game/df_game_r",        "10011", "/home/neople/game", 1, 1, 5, 5 },
        { "guild",      PROC_TYPE_GUILD,   "/home/neople/guild/df_guild_r",      "", "/home/neople/guild",   1, 1, 5, 5 },
        { "monitor",    PROC_TYPE_MONITOR, "/home/neople/monitor/df_monitor_r",  "", "/home/neople/monitor", 1, 1, 5, 3 },
    };

    int count = sizeof(configs) / sizeof(configs[0]);

    pthread_mutex_lock(&g_proc_mutex);
    for (int i = 0; i < count && g_process_count < MANAGER_MAX_PROCESSES; i++) {
        memcpy(&g_configs[g_process_count], &configs[i], sizeof(ProcessConfig));

        memset(&g_processes[g_process_count], 0, sizeof(ProcessInfo));
        g_processes[g_process_count].proc_id = g_next_proc_id++;
        strncpy(g_processes[g_process_count].name, configs[i].name, MANAGER_MAX_NAME_LEN);
        g_processes[g_process_count].type = configs[i].type;
        g_processes[g_process_count].status = PROC_STATUS_STOPPED;

        g_process_count++;
    }
    pthread_mutex_unlock(&g_proc_mutex);

    printf("[MANAGER] Loaded %d default service configurations\n", count);
}

/*==============================================================================
 * API Implementation
 *============================================================================*/

int manager_service_init(uint16_t port, const char* config_file) {
    g_port = port;
    memset(g_configs, 0, sizeof(g_configs));
    memset(g_processes, 0, sizeof(g_processes));

    if (config_file) {
        /* TODO: Load from config file */
        printf("[MANAGER] Config file loading not implemented, using defaults\n");
    }

    load_default_config();
    return 0;
}

void manager_service_cleanup(void) {
    cleanup_server();
}

int manager_service_run(void) {
    struct epoll_event events[MAX_EVENTS];
    uint32_t last_check = 0;
    uint32_t last_status = 0;

    printf("[MANAGER] Manager service running...\n");

    while (g_running) {
        int nfds = epoll_wait(g_epoll_fd, events, MAX_EVENTS, 1000);

        if (nfds < 0) {
            if (errno == EINTR) {
                if (g_child_exited) {
                    check_child_processes();
                    g_child_exited = 0;
                }
                continue;
            }
            printf("[MANAGER] epoll_wait error: %s\n", strerror(errno));
            break;
        }

        /* Handle child exits */
        if (g_child_exited) {
            check_child_processes();
            g_child_exited = 0;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == g_listen_fd) {
                accept_connection();
            }
        }

        /* Periodic checks */
        uint32_t now = (uint32_t)time(NULL);
        if (now - last_check >= CHECK_INTERVAL) {
            check_auto_restart();
            last_check = now;
        }

        if (now - last_status >= 60) {
            print_status();
            last_status = now;
        }
    }

    return 0;
}

int manager_start_service(const char* name) {
    return start_process(name);
}

int manager_stop_service(const char* name) {
    return stop_process(name, 0);
}

int manager_restart_service(const char* name) {
    stop_process(name, 0);
    sleep(1);
    return start_process(name);
}

int manager_kill_process(pid_t pid) {
    return kill(pid, SIGKILL);
}

int manager_get_status(const char* name, ProcessInfo* info) {
    if (!info) return -1;

    pthread_mutex_lock(&g_proc_mutex);
    ProcessInfo* proc = find_process(name);
    if (proc) {
        memcpy(info, proc, sizeof(ProcessInfo));
        pthread_mutex_unlock(&g_proc_mutex);
        return 0;
    }
    pthread_mutex_unlock(&g_proc_mutex);
    return -1;
}

int manager_get_all_status(ProcessInfo* info, int max_count) {
    if (!info) return -1;

    pthread_mutex_lock(&g_proc_mutex);
    int count = (g_process_count < max_count) ? g_process_count : max_count;
    memcpy(info, g_processes, count * sizeof(ProcessInfo));
    pthread_mutex_unlock(&g_proc_mutex);

    return count;
}

/*==============================================================================
 * Main Entry Point
 *============================================================================*/

static void print_usage(const char* prog) {
    printf("Usage: %s [port] [config_file]\n", prog);
    printf("  port: Server port (default: %d)\n", DEFAULT_PORT);
    printf("  config_file: Optional config file path\n");
}

int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("     DNF Manager Service v1.0\n");
    printf("========================================\n");

    const char* config_file = NULL;

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        g_port = (uint16_t)atoi(argv[1]);
    }

    if (argc > 2) {
        config_file = argv[2];
    }

    setup_signals();

    if (manager_service_init(g_port, config_file) != 0) {
        printf("[MANAGER] Failed to initialize service\n");
        return 1;
    }

    if (init_server() != 0) {
        printf("[MANAGER] Failed to initialize server\n");
        manager_service_cleanup();
        return 1;
    }

    int result = manager_service_run();

    manager_service_cleanup();
    printf("[MANAGER] Service stopped\n");

    return result;
}
