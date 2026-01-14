#
# Makefile - DNF Bridge Service
#
# Build: make all
# Clean: make clean
# Install: make install
#

# 编译器设置
CC = gcc
CFLAGS = -m32 -O2 -g -Wall -Wextra -pipe -std=gnu99
CFLAGS += -D_GNU_SOURCE
CFLAGS += -fPIC

# 包含路径
INCLUDES = -I. -I../common -I../../include

# 链接选项
LDFLAGS = -m32
LIBS = -lpthread -lrt

# 目标
TARGET = df_bridge_r

# 源文件
SOURCES = main.c bridge.c config.c network.c protocol.c
OBJECTS = $(SOURCES:.c=.o)

# 安装目录
INSTALL_DIR = ../../deploy/home/neople/bridge

#
# 构建规则
#

.PHONY: all clean install verify

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "=== Linking $(TARGET) ==="
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	@echo "=== Build complete: $(TARGET) ==="
	@ls -la $(TARGET)

%.o: %.c bridge.h
	@echo "=== Compiling $< ==="
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "=== Cleaning ==="
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.o

install: $(TARGET)
	@echo "=== Installing to $(INSTALL_DIR) ==="
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(INSTALL_DIR)/cfg
	mkdir -p $(INSTALL_DIR)/log
	mkdir -p $(INSTALL_DIR)/pid
	cp $(TARGET) $(INSTALL_DIR)/
	chmod +x $(INSTALL_DIR)/$(TARGET)
	@if [ -f cfg/bridge.cfg ]; then \
		cp cfg/bridge.cfg $(INSTALL_DIR)/cfg/; \
	fi
	@echo "=== Installation complete ==="

verify: $(TARGET)
	@echo "=== Verifying $(TARGET) ==="
	@file $(TARGET)
	@readelf -h $(TARGET) 2>/dev/null || echo "(readelf not available)"
	@echo ""
	@echo "=== Size ==="
	@ls -lh $(TARGET)
	@echo ""
	@echo "=== Dependencies ==="
	@ldd $(TARGET) 2>/dev/null || echo "(ldd not available)"

#
# Docker 构建
#
.PHONY: docker-build

docker-build:
	docker run --rm -v $(shell pwd)/../..:/build dnf_game_builder \
		make -C /build/src/bridge all

#
# 帮助
#
.PHONY: help

help:
	@echo "DNF Bridge Service Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build the bridge service"
	@echo "  clean        Remove build artifacts"
	@echo "  install      Install to deploy directory"
	@echo "  verify       Verify the built binary"
	@echo "  docker-build Build using Docker container"
	@echo "  help         Show this help"
	@echo ""
	@echo "Build requirements:"
	@echo "  - GCC with 32-bit support (-m32)"
	@echo "  - pthread library"
	@echo ""
