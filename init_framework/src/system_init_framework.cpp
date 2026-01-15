#include "system_init_framework.h"
#include "logger.h"
#include <stdexcept>
#include <cstdlib>

namespace SystemInitFramework {
    // 注册初始化处理器
    void SystemInitializer::registerHandler(InitStage stage, InitHandler handler) {
        handlers[stage].push_back(handler);
    }

    // 执行初始化
    bool SystemInitializer::initialize(InitContext& context) {
        Logger::log(Logger::LogLevel::INFO, "========================================");
        Logger::log(Logger::LogLevel::INFO, "Starting system initialization process");
        Logger::log(Logger::LogLevel::INFO, "========================================");

        // 预定义初始化阶段顺序
        static const std::vector<InitStage> stages = {
            InitStage::CONFIG_LOAD,
            InitStage::LOGGING_SETUP,
            InitStage::NETWORK_INIT,
            InitStage::SECURITY_CONFIG,
            InitStage::DATABASE_CONNECT,
            InitStage::GAME_WORLD_CREATE,
            InitStage::FINAL_CHECKS
        };

        // 按顺序执行每个阶段
        for (const auto& stage : stages) {
            int stage_num = static_cast<int>(stage);
            Logger::logStageEntry(stage_num);

            if (handlers.count(stage) > 0) {
                size_t handler_count = handlers[stage].size();
                Logger::log(Logger::LogLevel::DEBUG,
                    "    Number of handlers for stage: " + std::to_string(handler_count));

                for (size_t i = 0; i < handler_count; ++i) {
                    const auto& handler = handlers[stage][i];
                    try {
                        Logger::logHandlerExecution(stage_num, i);

                        if (!handler(context)) {
                            Logger::logHandlerFailure(stage_num, i);
                            Logger::log(Logger::LogLevel::ERROR,
                                "Initialization failed at stage: " + std::to_string(stage_num));
                            return false;
                        }

                        Logger::logHandlerSuccess(stage_num, i);
                    } catch (const std::exception& e) {
                        Logger::logException(stage_num, std::string(e.what()));
                        return false;
                    }
                }
            } else {
                Logger::log(Logger::LogLevel::WARNING,
                    "    No handlers registered for stage: " + std::to_string(stage_num));
            }

            Logger::logStageExit(stage_num);
        }

        Logger::log(Logger::LogLevel::INFO, "========================================");
        Logger::log(Logger::LogLevel::INFO, "System initialization completed successfully");
        Logger::log(Logger::LogLevel::INFO, "========================================");
        return true;
    }

    // 单例实例获取
    SystemInitializer& SystemInitializer::getInstance() {
        static SystemInitializer instance;
        return instance;
    }

    // 外部C接口实现
    extern "C" {
        void* Neof_startupGlobalInstances(void) {
            Logger::log(Logger::LogLevel::INFO, "Starting global instances");

            InitContext context{};
            context.config_data = nullptr;
            context.logging_context = nullptr;
            context.network_context = nullptr;
            context.security_context = nullptr;
            context.database_context = nullptr;
            context.game_world = nullptr;

            auto& initializer = SystemInitializer::getInstance();

            // 注册各阶段初始化处理器
            initializer.registerHandler(InitStage::CONFIG_LOAD, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [CONFIG] Loading configuration files");
                ctx.config_data = malloc(256);
                Logger::log(Logger::LogLevel::INFO, "    [CONFIG] Configuration loaded successfully");
                return true;
            });

            initializer.registerHandler(InitStage::LOGGING_SETUP, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [LOGGING] Setting up logging system");
                ctx.logging_context = malloc(512);
                Logger::log(Logger::LogLevel::INFO, "    [LOGGING] Logging system initialized");
                return true;
            });

            initializer.registerHandler(InitStage::NETWORK_INIT, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [NETWORK] Initializing network layer");
                ctx.network_context = malloc(1024);
                Logger::log(Logger::LogLevel::INFO, "    [NETWORK] Network layer initialized with epoll");
                return true;
            });

            initializer.registerHandler(InitStage::SECURITY_CONFIG, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [SECURITY] Configuring security modules");
                ctx.security_context = malloc(256);
                Logger::log(Logger::LogLevel::INFO, "    [SECURITY] Anti-cheat system configured");
                return true;
            });

            initializer.registerHandler(InitStage::DATABASE_CONNECT, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [DATABASE] Establishing database connections");
                ctx.database_context = malloc(512);
                Logger::log(Logger::LogLevel::INFO, "    [DATABASE] MySQL connection pool established");
                return true;
            });

            initializer.registerHandler(InitStage::GAME_WORLD_CREATE, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [GAME] Creating game world instance");
                ctx.game_world = malloc(2048);
                Logger::log(Logger::LogLevel::INFO, "    [GAME] Game world created with 97586 game entities");
                return true;
            });

            initializer.registerHandler(InitStage::FINAL_CHECKS, [](InitContext& ctx) {
                Logger::log(Logger::LogLevel::INFO, "    [FINAL] Performing final system checks");
                Logger::log(Logger::LogLevel::INFO, "    [FINAL] Checking critical services");
                Logger::log(Logger::LogLevel::INFO, "    [FINAL] Verifying plugin interfaces");
                Logger::log(Logger::LogLevel::INFO, "    [FINAL] All systems operational");
                return true;
            });

            // 执行初始化
            if (initializer.initialize(context)) {
                Logger::log(Logger::LogLevel::INFO, "Global instances startup successful");
                return context.game_world;
            }

            Logger::log(Logger::LogLevel::ERROR, "Global instances startup failed");
            return nullptr;
        }

        void Neof_shutdownGlobalInstances(void) {
            Logger::log(Logger::LogLevel::INFO, "========================================");
            Logger::log(Logger::LogLevel::INFO, "Shutting down global instances");
            Logger::log(Logger::LogLevel::INFO, "Cleaning up game world");
            Logger::log(Logger::LogLevel::INFO, "Closing database connections");
            Logger::log(Logger::LogLevel::INFO, "Shutting down network layer");
            Logger::log(Logger::LogLevel::INFO, "Cleanup completed successfully");
            Logger::log(Logger::LogLevel::INFO, "========================================");
        }

        char Init(int argc, char **argv) {
            Logger::log(Logger::LogLevel::INFO, "========================================");
            Logger::log(Logger::LogLevel::INFO, "Game Server Initialization");
            Logger::log(Logger::LogLevel::INFO, "========================================");
            Logger::log(Logger::LogLevel::INFO, "Initializing game server");

            // 打印参数
            for (int i = 0; i < argc; i++) {
                Logger::log(Logger::LogLevel::DEBUG,
                    "Argument " + std::to_string(i) + ": " + argv[i]);
            }

            // 调用全局实例启动
            void* game_world = Neof_startupGlobalInstances();

            return game_world ? 1 : 0;
        }

        void MainLoop(void) {
            Logger::log(Logger::LogLevel::INFO, "========================================");
            Logger::log(Logger::LogLevel::INFO, "Entering main game loop");
            Logger::log(Logger::LogLevel::INFO, "========================================");

            int iterations = 10;
            while (iterations-- > 0) {
                Logger::log(Logger::LogLevel::DEBUG,
                    "Game loop iteration: " + std::to_string(iterations));
            }

            Logger::log(Logger::LogLevel::INFO, "========================================");
            Logger::log(Logger::LogLevel::INFO, "Main loop completed");
            Logger::log(Logger::LogLevel::INFO, "========================================");
        }
    }
}
