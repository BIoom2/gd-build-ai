#pragma once

#include <cstdio>
#include <cstdarg>
#include <string>
#include <mutex>

namespace bloom::util {

// Lightweight logger - writes to a debug console (allocated by Bloom on init)
// plus an on-disk log file next to the DLL. Intentionally header-only and
// allocation-free on the hot path; the on-stack buffer caps each line at 1 KiB.
class Logger {
public:
    static Logger& instance();

    void init(const std::string& log_path);
    void shutdown();

    void log(const char* level, const char* fmt, ...);

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::mutex m_mutex;
    FILE* m_file = nullptr;
    bool m_console = false;
};

#define BLOOM_LOG_INFO(...)  ::bloom::util::Logger::instance().log("INFO ", __VA_ARGS__)
#define BLOOM_LOG_WARN(...)  ::bloom::util::Logger::instance().log("WARN ", __VA_ARGS__)
#define BLOOM_LOG_ERROR(...) ::bloom::util::Logger::instance().log("ERROR", __VA_ARGS__)
#define BLOOM_LOG_DEBUG(...) ::bloom::util::Logger::instance().log("DEBUG", __VA_ARGS__)

} // namespace bloom::util
