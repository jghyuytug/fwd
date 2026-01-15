CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -O2
INCLUDES = -I./include
SRCDIR = src
TESTDIR = test
BUILDDIR = build
BINDIR = bin

# 源文件
SOURCES = $(SRCDIR)/system_init_framework.cpp $(TESTDIR)/main.cpp
OBJECTS = $(BUILDDIR)/system_init_framework.o $(BUILDDIR)/main.o
TARGET = $(BINDIR)/game_server

# 默认目标
all: $(TARGET)

# 创建目录
directories:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)

# 编译对象文件
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR)/%.o: $(TESTDIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 链接可执行文件
$(TARGET): $(OBJECTS) | directories
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo "Build successful: $(TARGET)"

# 运行程序
run: $(TARGET)
	@echo "========================================"
	@echo "Running game server..."
	@echo "========================================"
	@./$(TARGET)

# 清理
clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(BINDIR)
	@echo "Clean completed"

# 重新构建
rebuild: clean all

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  all       - Build the game server"
	@echo "  run       - Build and run the game server"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and build"
	@echo "  help      - Show this help message"

.PHONY: all run clean rebuild help directories
