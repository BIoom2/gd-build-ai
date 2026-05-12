#pragma once

#include <string>

namespace bloom::core {

class Bloom {
public:
    static Bloom& instance();

    // Called from DllMain on PROCESS_ATTACH (from a worker thread).
    void start(const std::string& dll_dir);

    // Called from DllMain on PROCESS_DETACH or via Bloom's "shutdown" button.
    void stop();

    bool started() const { return m_started; }

private:
    Bloom() = default;
    Bloom(const Bloom&) = delete;
    Bloom& operator=(const Bloom&) = delete;

    bool m_started = false;
    std::string m_dll_dir;
};

} // namespace bloom::core
