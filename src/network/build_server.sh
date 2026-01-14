#!/bin/bash
#
# Build standalone game server for testing
#
# Usage:
#   ./build_server.sh           # Build server
#   ./build_server.sh clean     # Clean build artifacts
#   ./build_server.sh run       # Build and run server
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

case "$1" in
    clean)
        echo "=== Cleaning build artifacts ==="
        make clean
        ;;
    run)
        echo "=== Building and running server ==="
        make server
        echo ""
        echo "=== Starting server ==="
        ../build/game_server -f 7000
        ;;
    *)
        echo "=== Building standalone server ==="
        echo "Directory: $SCRIPT_DIR"
        echo ""
        make server
        echo ""
        echo "=== Build complete ==="
        echo "Binary: ../build/game_server"
        echo ""
        echo "To run the server:"
        echo "  ../build/game_server [options] [port]"
        echo ""
        echo "Options:"
        echo "  -f  Use full 13-byte header (production mode)"
        echo "  -l  Use legacy 4-byte header (default)"
        ;;
esac
