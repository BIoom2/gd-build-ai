#pragma once

#include <cstdint>
#include <string>

namespace bloom::util {

// IDA-style pattern scanner. Pattern uses hex bytes separated by spaces, with
// "?" or "??" marking wildcard bytes. Returns the absolute address of the first
// occurrence in [base, base + size), or 0 if not found.
//
//   auto addr = scan_pattern(reinterpret_cast<std::uintptr_t>(GetModuleHandleA("gd.dll")),
//                            module_size,
//                            "55 8B EC 6A FF 68 ?? ?? ?? ??");
std::uintptr_t scan_pattern(std::uintptr_t base, std::size_t size, const char* pattern);

// Convenience wrapper - scan the entire .text section of a loaded module.
std::uintptr_t scan_module(const char* module_name, const char* pattern);

} // namespace bloom::util
