# Test Programs Makefile

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c99 -g -O0

# Directories
SRC_DIR = ../src/resource
INC_DIR = ../include
PVF_SRC_DIR = ../../src/pvf
PVF_INC_DIR = ../../include

# Include paths
INCLUDES = -I$(INC_DIR) -I$(PVF_INC_DIR)

# Source files for resource module
RESOURCE_SOURCES = \
    $(SRC_DIR)/npk_parser.c \
    $(SRC_DIR)/ani_loader.c \
    $(SRC_DIR)/img_decoder.c \
    $(SRC_DIR)/cache_manager.c

# Minimal PVF sources needed for PackSet_Open / PackSet_ExtractFile
PVF_SOURCES = \
    $(PVF_SRC_DIR)/pvf_crc32.c \
    $(PVF_SRC_DIR)/pvf_decrypt.c \
    $(PVF_SRC_DIR)/pvf_hash.c \
    $(PVF_SRC_DIR)/pvf_parser.c \
    $(PVF_SRC_DIR)/pvf_packset.c \
    $(PVF_SRC_DIR)/pvf_stringtable.c

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
all: test_npk$(EXE) test_img_decoder$(EXE) test_ani_resolve$(EXE) list_pvf_ani$(EXE) dump_pvf_file$(EXE) test_ani_decode$(EXE)

# NPK Parser Test
test_npk$(EXE): test_npk.c $(SRC_DIR)/npk_parser.c
	@echo "Building test_npk..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ test_npk.c $(SRC_DIR)/npk_parser.c

# IMG Decoder Test
test_img_decoder$(EXE): test_img_decoder.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/img_decoder.c
	@echo "Building test_img_decoder..."
	$(CC) $(CFLAGS) $(INCLUDES) -DIMG_NO_SDL -o $@ test_img_decoder.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/img_decoder.c $(LIBS)

# ANI -> NPK resolve test (no rendering)
test_ani_resolve$(EXE): test_ani_resolve.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/ani_loader.c $(PVF_SOURCES)
	@echo "Building test_ani_resolve..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ test_ani_resolve.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/ani_loader.c $(PVF_SOURCES) $(LIBS)

# ANI -> IMG decode test (writes PPM; no SDL)
test_ani_decode$(EXE): test_ani_decode.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/ani_loader.c $(SRC_DIR)/img_decoder.c $(PVF_SOURCES)
	@echo "Building test_ani_decode..."
	$(CC) $(CFLAGS) $(INCLUDES) -DIMG_NO_SDL -o $@ test_ani_decode.c $(SRC_DIR)/npk_parser.c $(SRC_DIR)/ani_loader.c $(SRC_DIR)/img_decoder.c $(PVF_SOURCES) $(LIBS)

# PVF index lister (ANI only)
list_pvf_ani$(EXE): list_pvf_ani.c $(PVF_SOURCES)
	@echo "Building list_pvf_ani..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ list_pvf_ani.c $(PVF_SOURCES) $(LIBS)

# Dump PVF file bytes (format identification)
dump_pvf_file$(EXE): dump_pvf_file.c $(PVF_SOURCES)
	@echo "Building dump_pvf_file..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ dump_pvf_file.c $(PVF_SOURCES) $(LIBS)

# Run tests
run-npk: test_npk$(EXE)
	./test_npk$(EXE)

run-img: test_img_decoder$(EXE)
	./test_img_decoder$(EXE)

run-ani: test_ani_resolve$(EXE)
	@echo "Usage: ./test_ani_resolve$(EXE) <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]"

run-ani-decode: test_ani_decode$(EXE)
	@echo "Usage: ./test_ani_decode$(EXE) <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]"

run-list-ani: list_pvf_ani$(EXE)
	@echo "Usage: ./list_pvf_ani$(EXE) <Script.pvf> [contains] [limit]"

run-dump: dump_pvf_file$(EXE)
	@echo "Usage: ./dump_pvf_file$(EXE) <Script.pvf> <path_in_pvf> [bytes]"

# Clean
clean:
ifeq ($(OS),Windows_NT)
	-del /Q *.exe *.dll 2>nul
	-rmdir /S /Q output 2>nul
else
	-rm -f test_npk test_img_decoder test_ani_resolve test_ani_decode list_pvf_ani dump_pvf_file zlib1.dll
	-rm -rf output
endif

.PHONY: all clean run-npk run-img run-ani run-ani-decode run-list-ani run-dump
