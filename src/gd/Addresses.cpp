#include "gd/Addresses.hpp"

#include <windows.h>

namespace bloom::gd {

std::uintptr_t resolve_gd(std::uintptr_t rva) {
    static auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("gd.dll"));
    if (!base) {
        base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    }
    return base ? (base + rva) : 0;
}

} // namespace bloom::gd
