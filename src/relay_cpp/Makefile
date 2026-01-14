# DNF Relay Service Makefile
# Compatible with GCC 4.1.2 / CentOS 5.8

CC = gcc
CFLAGS = -m32 -O2 -g -Wall -Wno-unused-function -D_GNU_SOURCE -fPIC

TARGET = df_relay_r
SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -m32 -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
