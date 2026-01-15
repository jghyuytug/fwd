# Integration Tests Makefile

CC = gcc
CFLAGS = -m32 -Wall -Wextra -g -I../../include -I../../src
LDFLAGS = -m32

# Source files
TEST_SRCS = test_game_integration.c

# Dependencies from src/
INTEGRATION_SRCS = ../../src/integration/game_integration.c
EVENTS_SRCS = ../../src/events/events_manager.c
QUEST_SRCS = ../../src/quest/quest_manager.c ../../src/quest/quest_config.c
ACHIEVEMENT_SRCS = ../../src/achievement/achievement_manager.c
DUNGEON_SRCS = ../../src/dungeon/dungeon_manager.c

ALL_SRCS = $(TEST_SRCS) $(INTEGRATION_SRCS) $(EVENTS_SRCS) \
           $(QUEST_SRCS) $(ACHIEVEMENT_SRCS) $(DUNGEON_SRCS)

# Target
TARGET = test_integration

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(ALL_SRCS)
	$(CC) $(CFLAGS) -o $@ $(ALL_SRCS) $(LDFLAGS)
	@echo "[Test] Built: $@"

run: $(TARGET)
	@echo "Running integration tests..."
	./$(TARGET)

clean:
	rm -f $(TARGET)
	@echo "[Test] Clean complete"
