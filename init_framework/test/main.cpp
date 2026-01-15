#include "system_init_framework.h"
#include "logger.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[]) {
    Logger::log(Logger::LogLevel::INFO, "Game Server Application Started");
    Logger::log(Logger::LogLevel::INFO, "Version: 1.0.0");
    Logger::log(Logger::LogLevel::DEBUG, "Debug mode enabled");

    // 模拟游戏服务器启动流程
    char init_result = SystemInitFramework::Init(argc, argv);

    if (init_result) {
        Logger::log(Logger::LogLevel::INFO, "Server initialization successful, starting main loop");

        // 初始化成功，进入主循环
        SystemInitFramework::MainLoop();

        // 关闭全局实例
        SystemInitFramework::Neof_shutdownGlobalInstances();

        Logger::log(Logger::LogLevel::INFO, "Server shutdown completed successfully");
        return 0;
    } else {
        Logger::log(Logger::LogLevel::ERROR, "Server initialization failed");
        return 1;
    }
}
