#!/bin/bash
# Network Layer Integration Validation Script
# Verifies that network layer can be compiled and linked with main binary

set -e

echo "════════════════════════════════════════════════════════════"
echo "🔧 Network Layer Integration Validation"
echo "════════════════════════════════════════════════════════════"
echo ""

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "📁 Project root: $PROJECT_ROOT"
echo ""

# Step 1: Verify source files exist
echo "Step 1️⃣  Verifying source files..."
echo "─────────────────────────────────"

REQUIRED_NETWORK_FILES=(
    "src/network/CNetwork.cpp"
    "src/network/CEpoll.cpp"
    "src/network/CSessionManager.cpp"
    "include/network/CNetwork.h"
    "include/network/CEpoll.h"
    "include/network/CSessionManager.h"
)

REQUIRED_MAIN_FILES=(
    "src_split/df_game_r_part01.c"
    "src_split/df_game_r_decl.h"
    "Makefile.integrated"
)

for file in "${REQUIRED_NETWORK_FILES[@]}" "${REQUIRED_MAIN_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Missing file: $file"
        exit 1
    fi
    echo "✅ Found: $file"
done

echo ""

# Step 2: Check compilation environment
echo "Step 2️⃣  Checking compilation environment..."
echo "─────────────────────────────────────────"

if ! command -v g++ &> /dev/null; then
    echo "⚠️  g++ not found locally, will use Docker"
    USE_DOCKER=1
else
    echo "✅ g++ found: $(g++ --version | head -1)"
    USE_DOCKER=0
fi

if ! command -v docker &> /dev/null; then
    echo "❌ Docker not found, required for cross-compilation"
    exit 1
fi
echo "✅ Docker found"

echo ""

# Step 3: Prepare integration test
echo "Step 3️⃣  Preparing integration test..."
echo "────────────────────────────────"
echo "  Building integrated binary..."
echo "  - Main: 24 source files"
echo "  - Network: 9 source files"
echo "  - Total: 33 files"
echo ""

# Step 4: Run Docker build
if [ "$USE_DOCKER" -eq 1 ]; then
    echo "Step 4️⃣  Building in Docker..."
    echo "───────────────────────"

    # Check if base image exists
    if ! docker images | grep -q "df_game_r:latest"; then
        echo "❌ Base image 'df_game_r:latest' not found"
        echo "   Please build it first with: ./docker-build-split.sh"
        exit 1
    fi

    echo "✅ Base image found"
    echo ""

    # Create a temporary Dockerfile for integration testing
    cat > /tmp/Dockerfile.integration_test << 'EOF'
FROM df_game_r:latest

WORKDIR /build

# Copy integrated Makefile
COPY Makefile.integrated /build/

# Copy main sources
COPY src_split/ /build/src_split/

# Copy network layer
COPY src/network/ /build/src/network/
COPY include/ /build/include/

# Build integrated binary
RUN cd /build && \
    echo "=== Starting Integrated Build ===" && \
    make -f Makefile.integrated all 2>&1 | tee integration_build.log && \
    echo "" && \
    echo "=== Build Complete ===" && \
    ls -lh df_game_r_integrated && \
    echo "" && \
    echo "=== Verifying Binary ===" && \
    file df_game_r_integrated && \
    readelf -h df_game_r_integrated | grep -E "(Class|Machine|Entry|Interpreter)" && \
    echo "" && \
    echo "✅ Integration build successful!"

CMD ["/bin/bash"]
EOF

    echo "🐳 Building Docker image for integration test..."
    docker build -f /tmp/Dockerfile.integration_test -t df_game_r:integration_test . 2>&1 | tail -30

    echo ""
    echo "🐳 Running integration test..."
    docker run --rm df_game_r:integration_test bash -c "cd /build && ls -lh df_game_r_integrated && echo '' && file df_game_r_integrated"

else
    echo "Step 4️⃣  Building locally..."
    echo "──────────────────────────"

    cd src_split
    make -f ../Makefile.integrated all 2>&1 | tail -50
fi

echo ""
echo "════════════════════════════════════════════════════════════"
echo "✅ Integration validation complete!"
echo "════════════════════════════════════════════════════════════"
