# PVF浏览器后端编译脚本

CC = gcc
CFLAGS = -m32 -I../../../include -Wall -O2
LDFLAGS = -lz -lm -m32

SRCDIR = ../../../src/pvf
SOURCES = pvf_api.c \
          $(SRCDIR)/pvf_crc32.c \
          $(SRCDIR)/pvf_decrypt.c \
          $(SRCDIR)/pvf_hash.c \
          $(SRCDIR)/pvf_packset.c \
          $(SRCDIR)/pvf_parser.c \
          $(SRCDIR)/pvf_data_parser.c \
          $(SRCDIR)/parsers/equipment_parser.c

TARGET = pvf_api

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
	./$(TARGET) stats ../../../Script.pvf

.PHONY: all clean test
