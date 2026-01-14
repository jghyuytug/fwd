/**
 * @file game_main.c
 * @brief Game server main entry point
 *
 * Complete game server implementation using the engine framework.
 */

#include "../../engine.h"
#include "../../game/entity/entity.h"
#include "../../game/combat/combat_engine.h"
#include "../../game/buff/buff_system.h"
#include "../../game/ai/monster_ai.h"
#include "../../game/dungeon/dungeon_manager.h"
#include "../../script/lua_engine.h"
#include "../../script/hot_reload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* Server configuration */
#define SERVER_PORT         20011
#define SERVER_MAX_CLIENTS  1024
#define SERVER_TICK_RATE    50      /* 50ms = 20 ticks/second */

/* Game server context */
typedef struct {
    /* Core systems */
    EpollReactor        reactor;
    TCPListener         listener;
    SessionManager      sessions;
    MessageRouter       router;
    EventBus            events;
    TimerWheel          timers;
    Logger              logger;
    ConfigLoader        config;

    /* Game systems */
    BuffSystem          buff_system;
    DungeonManager      dungeons;

    /* Script engine */
    LuaEngine           lua;
    HotReload           hot_reload;

    /* IPC clients */
    IPCClient           guild_client;
    IPCClient           monitor_client;

    /* Server state */
    u16                 port;
    u32                 start_time;
    u32                 tick_count;
    u8                  running;
} GameServer;

/* Global server instance */
static GameServer g_server;

/* Signal handler */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[GameServer] Shutdown signal received\n");
    g_server.running = 0;
}

/* Timer callback for server tick */
static void on_tick(void* user_data)
{
    GameServer* server = (GameServer*)user_data;
    u32 current_time;

    (void)server;

    current_time = timer_wheel_now(&g_server.timers);

    /* Update sessions */
    /* session_manager_update(&g_server.sessions, current_time); */

    /* Update dungeons */
    /* Process active dungeon instances */

    /* Process event queue */
    event_bus_process(&g_server.events, 10);

    /* Update Lua engine (GC, hot reload) */
    lua_engine_update(&g_server.lua, current_time);
    hot_reload_check(&g_server.hot_reload, current_time);

    g_server.tick_count++;
}

/* Connection accepted callback */
static void on_accept(TCPListener* listener, int client_fd,
                     const char* client_ip, u16 client_port)
{
    Session* session;

    (void)listener;

    printf("[GameServer] Connection from %s:%u (fd=%d)\n",
           client_ip, client_port, client_fd);

    /* Create session */
    session = session_manager_create(&g_server.sessions, client_fd);
    if (!session) {
        printf("[GameServer] Failed to create session\n");
        close(client_fd);
        return;
    }

    strncpy(session->ip, client_ip, sizeof(session->ip) - 1);
    session->port = client_port;

    /* Add to reactor */
    /* epoll_reactor_add(&g_server.reactor, client_fd, ...); */
}

/* Initialize game server */
static int server_init(GameServer* server, u16 port, const char* config_path)
{
    int ret;

    memset(server, 0, sizeof(GameServer));
    server->port = port;
    server->running = 0;

    /* Initialize logger */
    ret = logger_init(&server->logger, LOG_LEVEL_DEBUG, "logs/game_server.log");
    if (ret != ENGINE_OK) {
        fprintf(stderr, "Failed to initialize logger\n");
        return ret;
    }
    LOG_INFO(&server->logger, "Game server initializing...");

    /* Load configuration */
    ret = config_loader_init(&server->config);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize config loader");
        return ret;
    }

    if (config_path) {
        ret = config_load_file(&server->config, config_path);
        if (ret != ENGINE_OK) {
            LOG_WARN(&server->logger, "Config file not found: %s", config_path);
        }
    }

    /* Initialize network */
    ret = epoll_reactor_init(&server->reactor, SERVER_MAX_CLIENTS);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize reactor");
        return ret;
    }

    ret = tcp_listener_init(&server->listener, port);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize listener on port %u", port);
        return ret;
    }

    server->listener.on_accept = on_accept;

    /* Initialize event system */
    ret = event_bus_init(&server->events);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize event bus");
        return ret;
    }

    /* Initialize timer system */
    ret = timer_wheel_init(&server->timers, 1000);  /* 1000ms base */
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize timer wheel");
        return ret;
    }

    /* Initialize session manager */
    ret = session_manager_init(&server->sessions, SERVER_MAX_CLIENTS);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize session manager");
        return ret;
    }

    /* Initialize message router */
    ret = message_router_init(&server->router);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize message router");
        return ret;
    }

    /* Initialize game systems */
    ret = buff_system_init(&server->buff_system);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize buff system");
        return ret;
    }

    ret = dungeon_manager_init(&server->dungeons);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to initialize dungeon manager");
        return ret;
    }

    /* Initialize Lua engine */
    ret = lua_engine_init(&server->lua);
    if (ret == ENGINE_OK) {
        lua_register_all(&server->lua);
        hot_reload_init(&server->hot_reload, &server->lua);

        /* Load game scripts */
        lua_engine_load_file(&server->lua, "scripts/init.lua");
        hot_reload_watch(&server->hot_reload, "scripts/init.lua");

        LOG_INFO(&server->logger, "Lua engine initialized");
    } else {
        LOG_WARN(&server->logger, "Lua engine not available: %s",
                 lua_engine_get_error(&server->lua));
    }

    /* Initialize IPC clients */
    ipc_client_init(&server->guild_client, IPC_SERVICE_GUILD, "127.0.0.1", 30403);
    ipc_client_init(&server->monitor_client, IPC_SERVICE_MONITOR, "127.0.0.1", 30303);

    LOG_INFO(&server->logger, "Game server initialized on port %u", port);

    return ENGINE_OK;
}

/* Cleanup game server */
static void server_cleanup(GameServer* server)
{
    LOG_INFO(&server->logger, "Game server shutting down...");

    /* Cleanup IPC */
    ipc_client_cleanup(&server->guild_client);
    ipc_client_cleanup(&server->monitor_client);

    /* Cleanup scripts */
    hot_reload_cleanup(&server->hot_reload);
    lua_engine_cleanup(&server->lua);

    /* Cleanup game systems */
    dungeon_manager_cleanup(&server->dungeons);
    buff_system_cleanup(&server->buff_system);

    /* Cleanup core systems */
    message_router_cleanup(&server->router);
    session_manager_cleanup(&server->sessions);
    timer_wheel_cleanup(&server->timers);
    event_bus_cleanup(&server->events);
    tcp_listener_cleanup(&server->listener);
    epoll_reactor_cleanup(&server->reactor);
    config_loader_cleanup(&server->config);

    LOG_INFO(&server->logger, "Game server shutdown complete");
    logger_cleanup(&server->logger);
}

/* Run game server */
static int server_run(GameServer* server)
{
    int ret;

    /* Start listening */
    ret = tcp_listener_start(&server->listener);
    if (ret != ENGINE_OK) {
        LOG_ERROR(&server->logger, "Failed to start listener");
        return ret;
    }

    /* Connect to services */
    if (ipc_client_connect(&server->monitor_client, 3000) == ENGINE_OK) {
        LOG_INFO(&server->logger, "Connected to Monitor service");
        /* Register with monitor */
    }

    if (ipc_client_connect(&server->guild_client, 3000) == ENGINE_OK) {
        LOG_INFO(&server->logger, "Connected to Guild service");
    }

    /* Add tick timer */
    timer_wheel_add(&server->timers, SERVER_TICK_RATE, on_tick, server, 1);

    server->start_time = timer_wheel_now(&server->timers);
    server->running = 1;

    LOG_INFO(&server->logger, "Game server running...");

    /* Main loop */
    while (server->running) {
        u32 now = timer_wheel_now(&server->timers);

        /* Process timers */
        timer_wheel_update(&server->timers, now);

        /* Process network (with timeout) */
        epoll_reactor_poll(&server->reactor, 10);  /* 10ms timeout */
    }

    return ENGINE_OK;
}

/* Main entry point */
int main(int argc, char* argv[])
{
    u16 port = SERVER_PORT;
    const char* config_path = "config/game.cfg";
    int ret;

    /* Parse command line */
    if (argc >= 2) {
        port = (u16)atoi(argv[1]);
    }
    if (argc >= 3) {
        config_path = argv[2];
    }

    printf("========================================\n");
    printf("  Game Server (Engine-based)\n");
    printf("  Port: %u\n", port);
    printf("========================================\n");

    /* Setup signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Initialize server */
    ret = server_init(&g_server, port, config_path);
    if (ret != ENGINE_OK) {
        fprintf(stderr, "Server initialization failed\n");
        return 1;
    }

    /* Run server */
    ret = server_run(&g_server);

    /* Cleanup */
    server_cleanup(&g_server);

    return ret == ENGINE_OK ? 0 : 1;
}
