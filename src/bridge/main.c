/*
 * main.c - DNF Bridge Service 入口点
 *
 * 用法: ./df_bridge_r <config_name> start
 * 例如: ./df_bridge_r bridge start
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "bridge.h"

#define DEFAULT_CONFIG_DIR  "cfg"
#define DEFAULT_CONFIG_FILE "bridge.cfg"
#define PID_DIR             "pid"
#define LOG_DIR             "log"

static bridge_server_t server;

/*
 * ensure_directory - 确保目录存在
 */
static int ensure_directory(const char* path)
{
    struct stat st;

    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return 0;
        }
        return -1;
    }

    return mkdir(path, 0755);
}

/*
 * write_pid_file - 写入 PID 文件
 */
static int write_pid_file(const char* name)
{
    char path[256];
    FILE* fp;

    ensure_directory(PID_DIR);

    snprintf(path, sizeof(path), "%s/%s.pid", PID_DIR, name);

    fp = fopen(path, "w");
    if (!fp) {
        bridge_log(LOG_ERROR, "main: cannot write PID file: %s", path);
        return -1;
    }

    fprintf(fp, "%d\n", getpid());
    fclose(fp);

    bridge_log(LOG_INFO, "main: PID file written: %s", path);

    return 0;
}

/*
 * remove_pid_file - 删除 PID 文件
 */
static void remove_pid_file(const char* name)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/%s.pid", PID_DIR, name);
    unlink(path);
}

/*
 * print_usage - 打印用法
 */
static void print_usage(const char* prog)
{
    fprintf(stderr, "DNF Bridge Service v%d.%d.%d\n",
            BRIDGE_VERSION_MAJOR, BRIDGE_VERSION_MINOR, BRIDGE_VERSION_PATCH);
    fprintf(stderr, "\nUsage: %s <config_name> <command>\n", prog);
    fprintf(stderr, "\nCommands:\n");
    fprintf(stderr, "  start    Start the bridge service\n");
    fprintf(stderr, "  stop     Stop the bridge service (send SIGTERM)\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s bridge start\n", prog);
    fprintf(stderr, "  %s bridge stop\n", prog);
    fprintf(stderr, "\nConfig file: %s/<config_name>.cfg\n", DEFAULT_CONFIG_DIR);
}

/*
 * main - 程序入口
 */
int main(int argc, char* argv[])
{
    char config_path[256];
    const char* config_name;
    const char* command;

    /* 检查参数 */
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    config_name = argv[1];
    command = argv[2];

    /* 构建配置文件路径 */
    snprintf(config_path, sizeof(config_path), "%s/%s.cfg",
             DEFAULT_CONFIG_DIR, config_name);

    /* 确保日志目录存在 */
    ensure_directory(LOG_DIR);

    /* 处理命令 */
    if (strcmp(command, "start") == 0) {
        bridge_log(LOG_INFO, "main: starting Bridge service...");
        bridge_log(LOG_INFO, "main: config=%s", config_path);

        /* 初始化服务 */
        if (bridge_init(&server, config_path) < 0) {
            bridge_log(LOG_ERROR, "main: initialization failed");
            return 1;
        }

        /* 写入 PID 文件 */
        write_pid_file(config_name);

        /* 运行主循环 */
        bridge_run(&server);

        /* 清理 */
        bridge_cleanup(&server);
        remove_pid_file(config_name);

        bridge_log(LOG_INFO, "main: Bridge service exited normally");
        return 0;
    }
    else if (strcmp(command, "stop") == 0) {
        char pid_path[256];
        FILE* fp;
        int pid;

        snprintf(pid_path, sizeof(pid_path), "%s/%s.pid", PID_DIR, config_name);

        fp = fopen(pid_path, "r");
        if (!fp) {
            fprintf(stderr, "Error: cannot read PID file: %s\n", pid_path);
            return 1;
        }

        if (fscanf(fp, "%d", &pid) != 1) {
            fprintf(stderr, "Error: invalid PID file format\n");
            fclose(fp);
            return 1;
        }
        fclose(fp);

        printf("Stopping Bridge service (PID %d)...\n", pid);

        if (kill(pid, SIGTERM) < 0) {
            perror("kill");
            return 1;
        }

        printf("Signal sent.\n");
        return 0;
    }
    else {
        fprintf(stderr, "Error: unknown command: %s\n", command);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
