#
# Makefile - DNF Bridge Service (C++ Version)
#
# Build: make all
# Clean: make clean
# Install: make install
#

# 编译器设置
CXX = g++
USE_32BIT ?= 1

CXX = g++

# 编译器设置
CXX = g++

ARCHFLAGS =
ifeq ($(USE_32BIT),1)
  ARCHFLAGS += -m32
endif

CXXFLAGS = $(ARCHFLAGS) -O2 -g -Wall -Wextra -pipe -std=c++11
CXXFLAGS += -D_GNU_SOURCE
CXXFLAGS += -fPIC

# 包含路径
INCLUDES = -I. -Iinclude -I../common -I../../include
ifeq ($(USE_32BIT),1)
  INCLUDES += -I/usr/include/c++/4.8.2/i686-redhat-linux
endif

# 链接选项
LDFLAGS = $(ARCHFLAGS)
LIBS = -lpthread -lrt

# 目标
TARGET = df_bridge_r

# 源文件目录
SRC_DIR = src
INC_DIR = include

# 源文件
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/packet_dump.cpp \
          $(SRC_DIR)/file_log.cpp \
          $(SRC_DIR)/linux_service.cpp \
          $(SRC_DIR)/tcp_user.cpp \
          $(SRC_DIR)/tcp_handler.cpp \
          $(SRC_DIR)/tcp_accept_thread.cpp \
          $(SRC_DIR)/user_pools.cpp \
          $(SRC_DIR)/channel_service.cpp \
          $(SRC_DIR)/crypto_aes.cpp \
          $(SRC_DIR)/crypto_des.cpp \
          $(SRC_DIR)/crypto_seed.cpp \
          $(SRC_DIR)/crypto_checksum.cpp \
          $(SRC_DIR)/crypto_factory.cpp \
          $(SRC_DIR)/check_thread.cpp \
          $(SRC_DIR)/script_thread.cpp \
          $(SRC_DIR)/server_group.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# 头文件
HEADERS = $(wildcard $(INC_DIR)/*.h)

# 安装目录
INSTALL_DIR = ../../deploy/home/neople/bridge

#
# 构建规则
#

.PHONY: all clean install verify docker-build help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "=== Linking $(TARGET) ==="
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	@echo "=== Build complete: $(TARGET) ==="
	@ls -la $(TARGET)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "=== Compiling $< ==="
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "=== Cleaning ==="
	rm -f $(OBJECTS) $(TARGET)
	rm -f $(SRC_DIR)/*.o

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
	@echo ""
	@echo "=== Symbol count ==="
	@nm $(TARGET) 2>/dev/null | wc -l || echo "(nm not available)"

#
# Docker 构建
#
docker-build:
	docker run --rm -v $(shell pwd)/../..:/build dnf_game_builder \
		make -C /build/src/bridge_cpp all

docker-verify:
	docker run --rm -v $(shell pwd)/../..:/build dnf_game_builder \
		make -C /build/src/bridge_cpp verify

#
# 帮助
#
help:
	@echo "DNF Bridge Service (C++ Version) Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build the bridge service"
	@echo "  clean        Remove build artifacts"
	@echo "  install      Install to deploy directory"
	@echo "  verify       Verify the built binary"
	@echo "  docker-build Build using Docker container"
	@echo "  docker-verify Verify using Docker container"
	@echo "  help         Show this help"
	@echo ""
	@echo "Build requirements:"
	@echo "  - G++ with 32-bit support (-m32)"
	@echo "  - C++11 support"
	@echo "  - pthread library"
	@echo ""
