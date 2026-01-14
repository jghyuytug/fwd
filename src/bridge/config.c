/*
 * config.c - Bridge 配置文件解析器
 *
 * 解析 bridge.cfg 格式:
 * [server]
 * max_client = 1000
 * this_ip = 127.0.0.1
 * this_udp_port = 7000
 * this_tcp_port = 7000
 *
 * [DB]
 * db_ip = 127.0.0.1
 * db_name = d_channel
 * db_id = game
 * db_pwd = uu5!^%jg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bridge.h"

/* 内部函数声明 */
static char* trim(char* str);
static int parse_line(const char* line, char* key, char* value);
static int set_config_value(bridge_config_t* config, const char* section,
                           const char* key, const char* value);

/*
 * bridge_load_config - 加载配置文件
 *
 * @path: 配置文件路径
 * @config: 配置结构指针
 *
 * 返回: 0成功, -1失败
 */
int bridge_load_config(const char* path, bridge_config_t* config)
{
    FILE* fp;
    char line[512];
    char section[64] = "";
    char key[128];
    char value[256];

    if (!path || !config) {
        bridge_log(LOG_ERROR, "config: invalid parameters");
        return -1;
    }

    /* 设置默认值 */
    memset(config, 0, sizeof(bridge_config_t));
    strncpy(config->listen_ip, BRIDGE_DEFAULT_IP, sizeof(config->listen_ip) - 1);
    config->tcp_port = BRIDGE_DEFAULT_PORT;
    config->udp_port = BRIDGE_DEFAULT_PORT;
    config->max_clients = BRIDGE_DEFAULT_MAX_CLIENT;
    config->db_port = 3306;
    config->channel_port = 7001;
    strncpy(config->channel_ip, "127.0.0.1", sizeof(config->channel_ip) - 1);

    fp = fopen(path, "r");
    if (!fp) {
        bridge_log(LOG_ERROR, "config: cannot open file: %s", path);
        return -1;
    }

    bridge_log(LOG_INFO, "config: loading %s", path);

    while (fgets(line, sizeof(line), fp)) {
        char* trimmed = trim(line);

        /* 跳过空行和注释 */
        if (trimmed[0] == '\0' || trimmed[0] == '#' || trimmed[0] == ';') {
            continue;
        }

        /* 检查节名 [section] */
        if (trimmed[0] == '[') {
            char* end = strchr(trimmed, ']');
            if (end) {
                *end = '\0';
                strncpy(section, trimmed + 1, sizeof(section) - 1);
                bridge_log(LOG_DEBUG, "config: section [%s]", section);
            }
            continue;
        }

        /* 解析 key = value */
        if (parse_line(trimmed, key, value) == 0) {
            set_config_value(config, section, key, value);
        }
    }

    fclose(fp);

    bridge_log(LOG_INFO, "config: loaded successfully");
    bridge_log(LOG_INFO, "config: listen=%s:%d (TCP/UDP)",
               config->listen_ip, config->tcp_port);
    bridge_log(LOG_INFO, "config: max_clients=%d", config->max_clients);
    bridge_log(LOG_INFO, "config: db=%s@%s/%s",
               config->db_user, config->db_ip, config->db_name);

    return 0;
}

/*
 * trim - 去除字符串首尾空白
 */
static char* trim(char* str)
{
    char* end;

    /* 去除前导空白 */
    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    /* 去除尾部空白 */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';

    return str;
}

/*
 * parse_line - 解析 key = value 行
 */
static int parse_line(const char* line, char* key, char* value)
{
    const char* eq = strchr(line, '=');
    if (!eq) {
        return -1;
    }

    /* 复制 key */
    int key_len = eq - line;
    if (key_len >= 128) key_len = 127;
    strncpy(key, line, key_len);
    key[key_len] = '\0';

    /* 去除 key 尾部空白 */
    char* k = trim(key);
    if (k != key) {
        memmove(key, k, strlen(k) + 1);
    }

    /* 复制 value */
    strncpy(value, eq + 1, 255);
    value[255] = '\0';

    /* 去除 value 首尾空白 */
    char* v = trim(value);
    if (v != value) {
        memmove(value, v, strlen(v) + 1);
    }

    return 0;
}

/*
 * set_config_value - 设置配置值
 */
static int set_config_value(bridge_config_t* config, const char* section,
                           const char* key, const char* value)
{
    bridge_log(LOG_DEBUG, "config: [%s] %s = %s", section, key, value);

    /* [server] 节 */
    if (strcasecmp(section, "server") == 0) {
        if (strcasecmp(key, "max_client") == 0) {
            config->max_clients = atoi(value);
        }
        else if (strcasecmp(key, "this_ip") == 0) {
            strncpy(config->listen_ip, value, sizeof(config->listen_ip) - 1);
        }
        else if (strcasecmp(key, "this_tcp_port") == 0) {
            config->tcp_port = atoi(value);
        }
        else if (strcasecmp(key, "this_udp_port") == 0) {
            config->udp_port = atoi(value);
        }
        else if (strcasecmp(key, "channel_ip") == 0) {
            strncpy(config->channel_ip, value, sizeof(config->channel_ip) - 1);
        }
        else if (strcasecmp(key, "channel_port") == 0) {
            config->channel_port = atoi(value);
        }
    }
    /* [DB] 节 */
    else if (strcasecmp(section, "DB") == 0) {
        if (strcasecmp(key, "db_ip") == 0) {
            strncpy(config->db_ip, value, sizeof(config->db_ip) - 1);
        }
        else if (strcasecmp(key, "db_name") == 0) {
            strncpy(config->db_name, value, sizeof(config->db_name) - 1);
        }
        else if (strcasecmp(key, "db_id") == 0) {
            strncpy(config->db_user, value, sizeof(config->db_user) - 1);
        }
        else if (strcasecmp(key, "db_pwd") == 0) {
            strncpy(config->db_pass, value, sizeof(config->db_pass) - 1);
        }
        else if (strcasecmp(key, "db_port") == 0) {
            config->db_port = atoi(value);
        }
    }

    return 0;
}
