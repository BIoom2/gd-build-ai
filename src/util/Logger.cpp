#include "util/Logger.hpp"

#include <cstring>
#include <ctime>

#include <windows.h>

namespace bloom::util {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::init(const std::string& log_path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file) return;

    if (AllocConsole()) {
        SetConsoleTitleA("Bloom Debug Console");
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        m_console = true;
    }

    fopen_s(&m_file, log_path.c_str(), "a");
    if (m_file) {
        std::fprintf(m_file, "\n===== Bloom session start =====\n");
        std::fflush(m_file);
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file) {
        std::fprintf(m_file, "===== Bloom session end =====\n");
        std::fclose(m_file);
        m_file = nullptr;
    }
    if (m_console) {
        FreeConsole();
        m_console = false;
    }
}

void Logger::log(const char* level, const char* fmt, ...) {
    char buffer[1024];

    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);
    int header = std::snprintf(buffer, sizeof(buffer),
                               "[%02d:%02d:%02d %s] ",
                               tm.tm_hour, tm.tm_min, tm.tm_sec, level);
    if (header < 0 || header >= static_cast<int>(sizeof(buffer))) header = 0;

    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buffer + header, sizeof(buffer) - header - 2, fmt, ap);
    va_end(ap);

    std::size_t end = std::strlen(buffer);
    if (end + 1 < sizeof(buffer)) {
        buffer[end] = '\n';
        buffer[end + 1] = '\0';
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_console) {
        std::fputs(buffer, stdout);
        std::fflush(stdout);
    }
    if (m_file) {
        std::fputs(buffer, m_file);
        std::fflush(m_file);
    }
}

} // namespace bloom::util
