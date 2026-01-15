#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Logger {
public:
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static void log(LogLevel level, const std::string& message) {
        std::time_t now = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        std::stringstream log_stream;
        log_stream << "[" << timestamp << "] ";

        switch (level) {
            case LogLevel::INFO:    log_stream << "[INFO] "; break;
            case LogLevel::WARNING: log_stream << "[WARNING] "; break;
            case LogLevel::ERROR:   log_stream << "[ERROR] "; break;
            case LogLevel::DEBUG:   log_stream << "[DEBUG] "; break;
        }

        log_stream << message;
        std::cout << log_stream.str() << std::endl;
    }

    static void logStageEntry(int stage) {
        log(LogLevel::INFO, ">>> Entering initialization stage: " + std::to_string(stage));
    }

    static void logStageExit(int stage) {
        log(LogLevel::INFO, "<<< Exiting initialization stage: " + std::to_string(stage));
    }

    static void logHandlerExecution(int stage, size_t handler_index) {
        log(LogLevel::DEBUG, "    Executing handler " + std::to_string(handler_index) +
            " for stage " + std::to_string(stage));
    }

    static void logHandlerSuccess(int stage, size_t handler_index) {
        log(LogLevel::DEBUG, "    Handler " + std::to_string(handler_index) +
            " completed successfully");
    }

    static void logHandlerFailure(int stage, size_t handler_index) {
        log(LogLevel::ERROR, "    Handler " + std::to_string(handler_index) +
            " failed at stage " + std::to_string(stage));
    }

    static void logException(int stage, const std::string& exception_msg) {
        log(LogLevel::ERROR, "    Exception at stage " + std::to_string(stage) +
            ": " + exception_msg);
    }
};

#endif // LOGGER_H
