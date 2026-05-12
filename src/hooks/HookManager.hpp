#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace bloom::hooks {

// Thin wrapper around MinHook so feature hooks read like plain function pointers.
// One global manager owns initialisation and global enable/disable.
class HookManager {
public:
    HookManager();
    ~HookManager();

    // Install a hook. `target` is the function in the game, `detour` is your
    // replacement, and `out_original` is a pointer to a function pointer that
    // will receive the trampoline you call to invoke the original. Returns
    // true on success.
    bool install(void* target, void* detour, void** out_original, const char* tag);

    // Enable / disable everything that's been installed.
    bool enable_all();
    bool disable_all();

    // Stats / diagnostics.
    std::size_t installed_count() const { return m_installed.size(); }

    static HookManager& instance();

private:
    struct Entry {
        void* target;
        void* detour;
        std::string tag;
    };
    std::vector<Entry> m_installed;
    bool m_initialized = false;
};

} // namespace bloom::hooks
