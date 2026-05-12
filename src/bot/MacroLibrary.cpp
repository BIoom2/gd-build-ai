#include "bot/MacroLibrary.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "util/Logger.hpp"

namespace bloom::bot {

namespace fs = std::filesystem;

void MacroLibrary::init(const std::string& dir) {
    m_dir = dir;
    std::error_code ec;
    fs::create_directories(m_dir, ec);
    refresh();
}

void MacroLibrary::refresh() {
    m_names.clear();
    if (m_dir.empty()) return;

    std::error_code ec;
    if (!fs::exists(m_dir, ec)) return;
    for (const auto& entry : fs::directory_iterator(m_dir, ec)) {
        if (ec) break;
        if (!entry.is_regular_file()) continue;
        auto path = entry.path();
        if (path.extension() != ".json") continue;
        m_names.push_back(path.stem().string());
    }
    std::sort(m_names.begin(), m_names.end());
    BLOOM_LOG_INFO("MacroLibrary refreshed: %zu macros", m_names.size());
}

std::vector<std::string> MacroLibrary::filtered(const std::string& query) const {
    if (query.empty()) return m_names;

    auto to_lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return s;
    };
    auto q = to_lower(query);

    std::vector<std::string> out;
    out.reserve(m_names.size());
    for (const auto& n : m_names) {
        if (to_lower(n).find(q) != std::string::npos) {
            out.push_back(n);
        }
    }
    return out;
}

bool MacroLibrary::save(const Macro& macro) const {
    if (macro.name.empty()) return false;
    auto path = path_for(macro.name);
    return macro.save(path);
}

std::optional<Macro> MacroLibrary::load(const std::string& name) const {
    Macro m;
    if (Macro::load(path_for(name), m)) return m;
    return std::nullopt;
}

bool MacroLibrary::exists(const std::string& name) const {
    std::error_code ec;
    return fs::exists(path_for(name), ec);
}

std::string MacroLibrary::path_for(const std::string& name) const {
    fs::path p = m_dir;
    p /= (name + ".json");
    return p.string();
}

MacroLibrary& get_macro_library() {
    static MacroLibrary lib;
    return lib;
}

} // namespace bloom::bot
