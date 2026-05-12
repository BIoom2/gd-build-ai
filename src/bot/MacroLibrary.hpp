#pragma once

#include <optional>
#include <string>
#include <vector>

#include "bot/Macro.hpp"

namespace bloom::bot {

// Lists JSON macros under `bloom_macros/`. Supports the search dropdown
// (screenshot 3) by exposing a filtered view of the available names.
class MacroLibrary {
public:
    // Initialise with the absolute directory where macros live.
    void init(const std::string& dir);

    // Re-scan the directory.
    void refresh();

    const std::vector<std::string>& names() const { return m_names; }

    // Names matching `query` (substring, case-insensitive). Pass empty query
    // to get everything.
    std::vector<std::string> filtered(const std::string& query) const;

    bool save(const Macro& macro) const;
    std::optional<Macro> load(const std::string& name) const;
    bool exists(const std::string& name) const;
    std::string path_for(const std::string& name) const;

    const std::string& directory() const { return m_dir; }

private:
    std::string m_dir;
    std::vector<std::string> m_names;
};

MacroLibrary& get_macro_library();

} // namespace bloom::bot
