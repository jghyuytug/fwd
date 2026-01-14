# PVF模块 Makefile
# 用于编译PVF解析库

# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -m32 -fPIC
INCLUDES = -I../../include

# 目录
BUILD_DIR = ../../build/pvf
LIB_DIR = ../lib

# 源文件
CORE_SRCS = pvf_crc32.c pvf_decrypt.c pvf_hash.c pvf_parser.c \
            pvf_packset.c pvf_stringtable.c pvf_data_parser.c pvf_manager.c
PARSER_SRCS = parsers/parser_common.c parsers/equipment_parser.c

ALL_SRCS = $(CORE_SRCS) $(PARSER_SRCS)

# 目标文件
CORE_OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(CORE_SRCS))
PARSER_OBJS = $(patsubst parsers/%.c,$(BUILD_DIR)/parsers/%.o,$(PARSER_SRCS))
ALL_OBJS = $(CORE_OBJS) $(PARSER_OBJS)

# 库文件
STATIC_LIB = $(LIB_DIR)/libpvf.a

# 默认目标
.PHONY: all clean dirs verify test

all: dirs $(STATIC_LIB)

# 创建目录
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/parsers
	@mkdir -p $(LIB_DIR)

# 静态库
$(STATIC_LIB): $(ALL_OBJS)
	@echo "[AR] Creating $@"
	@ar rcs $@ $^
	@echo "[OK] PVF library built successfully"

# 编译核心源文件
$(BUILD_DIR)/%.o: %.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 编译解析器源文件
$(BUILD_DIR)/parsers/%.o: parsers/%.c
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 清理
clean:
	@echo "[CLEAN] Removing build files"
	@rm -rf $(BUILD_DIR)
	@rm -f $(STATIC_LIB)

# 验证
verify:
	@echo "=== PVF Module Verification ==="
	@echo "Source files: $(words $(ALL_SRCS))"
	@echo "Object files: $(words $(ALL_OBJS))"
	@if [ -f $(STATIC_LIB) ]; then \
		echo "Library: $(STATIC_LIB) [OK]"; \
		ar -t $(STATIC_LIB) | wc -l | xargs -I {} echo "  Objects in library: {}"; \
	else \
		echo "Library: $(STATIC_LIB) [NOT BUILT]"; \
	fi

# 测试编译（不链接）
test: dirs
	@echo "=== Testing PVF Module Compilation ==="
	@for src in $(ALL_SRCS); do \
		echo "[TEST] $$src"; \
		$(CC) $(CFLAGS) $(INCLUDES) -c $$src -o /dev/null 2>&1 || echo "  [FAIL] $$src"; \
	done
	@echo "[DONE] Compilation test complete"

# 显示信息
info:
	@echo "=== PVF Module Info ==="
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Includes: $(INCLUDES)"
	@echo ""
	@echo "Core sources:"
	@for f in $(CORE_SRCS); do echo "  $$f"; done
	@echo ""
	@echo "Parser sources:"
	@for f in $(PARSER_SRCS); do echo "  $$f"; done
