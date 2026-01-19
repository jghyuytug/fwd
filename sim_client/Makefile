# Sim client Makefile (Windows/MSYS2 MinGW)

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c99 -O0 -g

SIM_DIR = .
SRC_DIR = $(SIM_DIR)/src
BUILD_DIR = $(SIM_DIR)/build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(SIM_DIR)/bin

EXE =
ifeq ($(OS),Windows_NT)
    EXE = .exe
endif

TARGET = $(BIN_DIR)/sim_client$(EXE)

# Reuse existing modules from repo
CLIENT_RESOURCE_DIR = ../client/src/resource
CLIENT_INCLUDE_DIR = ../client/include
ROOT_INCLUDE_DIR = ../include

PVF_SRC_DIR = ../src/pvf

INCLUDES = -I$(SRC_DIR) -I$(CLIENT_INCLUDE_DIR) -I$(ROOT_INCLUDE_DIR)

DEFS = -DIMG_NO_SDL=1
LIBS = -lz

PVF_SOURCES = \
    $(PVF_SRC_DIR)/pvf_crc32.c \
    $(PVF_SRC_DIR)/pvf_decrypt.c \
    $(PVF_SRC_DIR)/pvf_hash.c \
    $(PVF_SRC_DIR)/pvf_parser.c \
    $(PVF_SRC_DIR)/pvf_packset.c \
    $(PVF_SRC_DIR)/pvf_stringtable.c

RESOURCE_SOURCES = \
    $(CLIENT_RESOURCE_DIR)/ani_loader.c \
    $(CLIENT_RESOURCE_DIR)/npk_parser.c \
    $(CLIENT_RESOURCE_DIR)/img_decoder.c

SIM_SOURCES = \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/sim_app.c

SOURCES = $(SIM_SOURCES) $(RESOURCE_SOURCES) $(PVF_SOURCES)

OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))

vpath %.c $(SRC_DIR) $(CLIENT_RESOURCE_DIR) $(PVF_SRC_DIR)

all: dirs $(TARGET)

dirs:
	@powershell -NoProfile -Command "New-Item -ItemType Directory -Force '$(OBJ_DIR)' | Out-Null; New-Item -ItemType Directory -Force '$(BIN_DIR)' | Out-Null"

$(TARGET): $(OBJECTS)
	@echo Linking $@
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: %.c
	@echo Compiling $<
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

clean:
	@powershell -NoProfile -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }; if (Test-Path '$(BIN_DIR)') { Remove-Item -Recurse -Force '$(BIN_DIR)' }"

.PHONY: all clean dirs

