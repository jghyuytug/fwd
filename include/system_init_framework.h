#ifndef SYSTEM_INIT_FRAMEWORK_H
#define SYSTEM_INIT_FRAMEWORK_H

#include <cstdint>
#include <functional>
#include <map>
#include <vector>

namespace SystemInitFramework {
    enum class InitStage {
        CONFIG_LOAD = 0,
        LOGGING_SETUP,
        NETWORK_INIT,
        SECURITY_CONFIG,
        DATABASE_CONNECT,
        GAME_WORLD_CREATE,
        FINAL_CHECKS
    };

    struct InitContext {
        void* config_data;
        void* logging_context;
        void* network_context;
        void* security_context;
        void* database_context;
        void* game_world;
    };

    using InitHandler = std::function<bool(InitContext&)>;

    class SystemInitializer {
    public:
        void registerHandler(InitStage stage, InitHandler handler);
        bool initialize(InitContext& context);
        static SystemInitializer& getInstance();

    private:
        SystemInitializer() = default;
        std::map<InitStage, std::vector<InitHandler>> handlers;
    };

    extern "C" {
        void* Neof_startupGlobalInstances(void);
        void Neof_shutdownGlobalInstances(void);
        char Init(int argc, char **argv);
        void MainLoop(void);
    }
}

// 为 InitStage 提供 std::hash 特化
namespace std {
    template <>
    struct hash<SystemInitFramework::InitStage> {
        size_t operator()(const SystemInitFramework::InitStage& stage) const {
            return static_cast<size_t>(stage);
        }
    };
}

#endif // SYSTEM_INIT_FRAMEWORK_H