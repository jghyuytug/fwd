#ifndef SYSTEM_INIT_FRAMEWORK_H
#define SYSTEM_INIT_FRAMEWORK_H

#include <cstdint>
#include <functional>
#include <map>
#include <vector>
#include <iostream>
#include <string>

namespace SystemInitFramework {
    // 保留原有的初始化阶段枚举
    enum class InitStage {
        CONFIG_LOAD = 0,
        LOGGING_SETUP,
        NETWORK_INIT,
        SECURITY_CONFIG,
        DATABASE_CONNECT,
        GAME_WORLD_CREATE,
        FINAL_CHECKS
    };

    // 初始化上下文结构体
    struct InitContext {
        void* config_data;
        void* logging_context;
        void* network_context;
        void* security_context;
        void* database_context;
        void* game_world;
    };

    // 初始化处理器类型定义
    using InitHandler = std::function<bool(InitContext&)>;

    // 系统初始化器类
    class SystemInitializer {
    public:
        // 注册初始化处理器
        void registerHandler(InitStage stage, InitHandler handler);

        // 执行初始化
        bool initialize(InitContext& context);

        // 单例模式获取实例
        static SystemInitializer& getInstance();

    private:
        // 私有构造函数
        SystemInitializer() = default;

        // 存储各阶段的处理器
        std::map<InitStage, std::vector<InitHandler>> handlers;
    };

    // 外部C接口声明
    extern "C" {
        void* Neof_startupGlobalInstances(void);
        void Neof_shutdownGlobalInstances(void);
        char Init(int argc, char **argv);
        void MainLoop(void);
    }
}

#endif // SYSTEM_INIT_FRAMEWORK_H
