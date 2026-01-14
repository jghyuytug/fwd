/**
 * @file server_main.c
 * @brief DNF Game Server - Main Entry Point
 *
 * Simple test server to verify network implementation.
 *
 * Usage:
 *   ./game_server [port]
 *
 * Default port: 7000
 *
 * @version 1.0
 * @date 2025-11-26
 */

#include "game_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

static volatile int g_running = 1;

static void SignalHandler(int sig)
{
    printf("\n[Main] Received signal %d, shutting down...\n", sig);
    g_running = 0;
    GameServer_Stop();
}

static void PrintUsage(const char* program)
{
    printf("DNF Game Server - Network Test\n");
    printf("==============================\n");
    printf("Usage: %s [options] [port]\n", program);
    printf("\n");
    printf("Options:\n");
    printf("  -f, --full    Use full 13-byte protocol header (production mode)\n");
    printf("  -l, --legacy  Use legacy 4-byte protocol header (default)\n");
    printf("  -h, --help    Show this help message\n");
    printf("\n");
    printf("  port: TCP port to listen on (default: 7000)\n");
    printf("\n");
    printf("Protocol Header Formats:\n");
    printf("  Legacy (4 bytes):  [length:2][type:2]\n");
    printf("  Full (13 bytes):   [class:1][length:4][msg:1][pad:1][checksum:4][seq:2]\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s 7000           # Legacy mode on port 7000\n", program);
    printf("  %s -f 7000        # Full header mode on port 7000\n", program);
    printf("\n");
}

int main(int argc, char* argv[])
{
    int port = 7000;
    int use_full_header = 0;

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--full") == 0) {
            use_full_header = 1;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--legacy") == 0) {
            use_full_header = 0;
        } else {
            /* Assume it's a port number */
            port = atoi(argv[i]);
            if (port <= 0 || port > 65535) {
                fprintf(stderr, "Invalid port: %s\n", argv[i]);
                return 1;
            }
        }
    }

    printf("=====================================\n");
    printf("DNF Game Server - Network Test\n");
    printf("=====================================\n");
    printf("Port: %d\n", port);
    printf("Protocol: %s (%d-byte header)\n",
           use_full_header ? "Full" : "Legacy",
           use_full_header ? 13 : 4);
    printf("Press Ctrl+C to stop\n");
    printf("=====================================\n\n");

    /* Set protocol mode */
    GameServer_SetProtocolMode(use_full_header);

    /* Setup signal handlers */
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGPIPE, SIG_IGN);  /* Ignore broken pipe */

    /* Initialize server */
    if (GameServer_Initialize(port) < 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    printf("\n[Main] Server ready. Waiting for connections...\n\n");

    /* Run server */
    int result = GameServer_Run();

    /* Cleanup */
    GameServer_Cleanup();

    printf("\n[Main] Server stopped.\n");
    return result;
}
