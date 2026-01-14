# Test Programs Makefile

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c99 -g -O0

# Directories
SRC_DIR = ../src/resource
INC_DIR = ../include

# Include paths
INCLUDES = -I$(INC_DIR)

# Source files for resource module
RESOURCE_SOURCES = \
    $(SRC_DIR)/npk_parser.c \
    $(SRC_DIR)/img_decoder.c \
    $(SRC_DIR)/cache_manager.c

# Detect OS
ifeq ($(OS),Windows_NT)
    EXE = .exe
    RM = del /Q
    LIBS = -lz
else
    EXE =
    RM = rm -f
    LIBS = -lz
endif

# Targets
all: test_npk$(EXE) test_img$(EXE)

# NPK Parser Test
test_npk$(EXE): test_npk.c $(SRC_DIR)/npk_parser.c
	@echo "Building test_npk..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ test_npk.c $(SRC_DIR)/npk_parser.c

# IMG Decoder Test
test_img$(EXE): test_img.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/img_decoder.c
	@echo "Building test_img..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ test_img.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/img_decoder.c $(LIBS)

# Run tests
run-npk: test_npk$(EXE)
	./test_npk$(EXE)

run-img: test_img$(EXE)
	./test_img$(EXE)

# Clean
clean:
ifeq ($(OS),Windows_NT)
	-del /Q *.exe 2>nul
else
	-rm -f test_npk test_img
endif

.PHONY: all clean run-npk run-img
