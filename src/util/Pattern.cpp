#include "util/Pattern.hpp"

#include <vector>

#include <windows.h>

namespace bloom::util {

namespace {

struct PatternByte {
    std::uint8_t value;
    bool wildcard;
};

std::vector<PatternByte> parse(const char* pattern) {
    std::vector<PatternByte> out;
    out.reserve(64);

    for (const char* p = pattern; *p; ) {
        while (*p == ' ') ++p;
        if (!*p) break;

        if (*p == '?') {
            out.push_back({0, true});
            while (*p == '?') ++p;
            continue;
        }

        auto hex = [](char c) -> int {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
            if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
            return -1;
        };

        int hi = hex(*p++);
        if (hi < 0) break;
        int lo = hex(*p);
        if (lo < 0) {
            out.push_back({static_cast<std::uint8_t>(hi), false});
        } else {
            ++p;
            out.push_back({static_cast<std::uint8_t>((hi << 4) | lo), false});
        }
    }
    return out;
}

} // namespace

std::uintptr_t scan_pattern(std::uintptr_t base, std::size_t size, const char* pattern) {
    auto bytes = parse(pattern);
    if (bytes.empty() || size < bytes.size()) return 0;

    const auto* mem = reinterpret_cast<const std::uint8_t*>(base);
    const std::size_t last = size - bytes.size();
    for (std::size_t i = 0; i <= last; ++i) {
        bool ok = true;
        for (std::size_t j = 0; j < bytes.size(); ++j) {
            if (bytes[j].wildcard) continue;
            if (mem[i + j] != bytes[j].value) { ok = false; break; }
        }
        if (ok) return base + i;
    }
    return 0;
}

std::uintptr_t scan_module(const char* module_name, const char* pattern) {
    HMODULE mod = GetModuleHandleA(module_name);
    if (!mod) return 0;

    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(mod);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<std::uintptr_t>(mod) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

    return scan_pattern(reinterpret_cast<std::uintptr_t>(mod),
                        nt->OptionalHeader.SizeOfImage,
                        pattern);
}

} // namespace bloom::util
