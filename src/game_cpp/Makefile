# DNF Game Server (Extended) Makefile
# Compatible with GCC 4.1.2 / CentOS 5.8

CXX = g++
CC = gcc
CXXFLAGS = -m32 -O2 -g -Wall -Wno-unused-function -D_GNU_SOURCE -fPIC
CFLAGS = -m32 -O2 -g -Wall -Wno-unused-function -D_GNU_SOURCE -fPIC -std=c99

# Include paths
INCLUDES = -Iinclude -I../include -I.. \
           -I../database/headers -I../../include/database/headers \
           -I./orm/core_game/taiwan_cain/headers \
           -I./orm/core_game/d_taiwan/headers \
           -I./orm/core_game/d_guild/headers \
           -I./orm/extended_features/taiwan_cain_2nd/headers
MYSQL_INC = -I/usr/include/mysql

# Libraries
MYSQL_LIBS = -L/usr/lib/mysql -lmysqlclient -L/usr/lib -lz -lm

# Source directories for modular build
SRC_ROOT = src
SRC_DB_DIR = $(SRC_ROOT)/db
SRC_HANDLER_DIR = $(SRC_ROOT)/handlers
SRC_SYSTEM_DIR = $(SRC_ROOT)/systems

DB_MODULE_SRCS = $(wildcard $(SRC_DB_DIR)/*.cpp)
HANDLER_SRCS = $(wildcard $(SRC_HANDLER_DIR)/*.cpp)
SYSTEM_SRCS = $(wildcard $(SRC_SYSTEM_DIR)/*.cpp)

# Shared sources
COMMON_CPP_SRCS = src/game_packet_utils.cpp

# Standard version (mock data)
TARGET = df_game_r
SRCS = src/main.cpp $(COMMON_CPP_SRCS)
OBJS = $(SRCS:.cpp=.o)

# Database version (MySQL + ORM integration)
TARGET_DB = df_game_db
DB_CPP_SRCS = src/main_db.cpp $(COMMON_CPP_SRCS) $(DB_MODULE_SRCS) $(HANDLER_SRCS) $(SYSTEM_SRCS)
DB_C_SRCS = ../database/db_connection_manager.c \
            ./orm/core_game/taiwan_cain/sources/charac_info_orm.c \
            ./orm/core_game/taiwan_cain/sources/charac_friends_orm.c \
            ./orm/core_game/d_taiwan/sources/accounts_orm.c \
            ./orm/core_game/d_guild/sources/guild_info_orm.c \
            ./orm/core_game/d_guild/sources/guild_member_orm.c \
            ./orm/extended_features/taiwan_cain_2nd/sources/inventory_orm.c \
            ./orm/extended_features/taiwan_cain_2nd/sources/skill_orm.c \
            ./orm/extended_features/taiwan_cain_2nd/sources/user_items_orm.c
SRCS_DB = $(DB_CPP_SRCS) $(DB_C_SRCS)
OBJS_DB = $(patsubst %.cpp,%.o,$(DB_CPP_SRCS)) $(patsubst %.c,%.o,$(DB_C_SRCS))
MAIN_DB_OBJ = src/main_db.o
COMMON_DB_OBJS = $(filter-out $(MAIN_DB_OBJ),$(OBJS_DB))

# ORM Adapter version (uses ORM layer)
TARGET_ORM = df_game_orm
SRCS_ORM = $(SRCS_DB) src/orm_adapter.c
OBJS_ORM = src/main_db_orm.o $(COMMON_DB_OBJS) src/orm_adapter.o

all: $(TARGET)

# Standard build
$(TARGET): $(OBJS)
	$(CXX) -m32 -o $@ $^ -lpthread

# Database build (direct MySQL queries)
db: $(TARGET_DB)

$(TARGET_DB): $(OBJS_DB)
	$(CXX) -m32 -o $@ $^ -lpthread $(MYSQL_LIBS)

# ORM build (uses ORM adapter layer)
orm: $(TARGET_ORM)

$(TARGET_ORM): $(OBJS_ORM)
	$(CXX) -m32 -o $@ $^ -lpthread $(MYSQL_LIBS)

src/main_db.o: src/main_db.cpp
	$(CXX) $(CXXFLAGS) $(MYSQL_INC) $(INCLUDES) -c $< -o $@

src/main_db_orm.o: src/main_db.cpp
	$(CXX) $(CXXFLAGS) $(MYSQL_INC) $(INCLUDES) -DUSE_ORM_ADAPTER -c $< -o $@

src/orm_adapter.o: src/orm_adapter.c
	$(CC) $(CFLAGS) $(MYSQL_INC) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(MYSQL_INC) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(MYSQL_INC) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(OBJS_DB) $(TARGET_DB) $(OBJS_ORM) $(TARGET_ORM)

.PHONY: all db orm clean
