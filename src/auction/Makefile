# Makefile for DNF Auction Service
# Phase 16 P4: Independent Auction Service

CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -std=c99
LDFLAGS = -lpthread

# Windows support
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
    EXE = df_auction.exe
else
    EXE = df_auction
endif

# Source files
SRCS = main.c \
       auction_manager.c \
       auction_handler.c \
       auction_ipc.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HDRS = auction_types.h \
       auction_manager.h \
       auction_handler.h \
       auction_ipc.h

# Default target
all: $(EXE)

# Link
$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(EXE)

# Install (copy to bin directory)
install: $(EXE)
	mkdir -p ../../bin
	cp $(EXE) ../../bin/

# Run
run: $(EXE)
	./$(EXE)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: clean all

# Release build
release: CFLAGS += -DNDEBUG -O3
release: clean all

.PHONY: all clean install run debug release
