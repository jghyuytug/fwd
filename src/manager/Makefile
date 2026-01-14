# DNF Manager Service Makefile
# Compatible with GCC 4.1.2 / CentOS 5.8

CC = gcc
CFLAGS = -m32 -O2 -g -Wall -D_GNU_SOURCE -fPIC -std=gnu99

# Include paths
INCLUDES = -Iinclude

# Libraries
LIBS = -lpthread

# Target
TARGET = df_manager_r
SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -m32 -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /home/dxf/game/manager/

.PHONY: all clean install
