# DNF Guild Service Makefile
# Compatible with GCC 4.1.2 / CentOS 5.8

CC = gcc
CFLAGS = -m32 -O2 -g -Wall -D_GNU_SOURCE -fPIC -std=gnu99

# Include paths
INCLUDES = -Iinclude -I/usr/include/mysql

# Libraries
LIBS = -lpthread -L/usr/lib/mysql -lmysqlclient -L/usr/lib -lz

# Target
TARGET = df_guild_r
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
	cp $(TARGET) /home/dxf/game/guild/

.PHONY: all clean install
