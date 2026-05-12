cmake_minimum_required(VERSION 3.21)

# -----------------------------------------------------------------------------
# Bloom - a TAS / macro bot for Geometry Dash 2.1 (Windows x86).
#
# Build output is a single Bloom.dll that is loaded into GeometryDash.exe via
# any standard DLL injector. The DLL hosts an ImGui (DX9) menu, hooks key
# PlayLayer / PlayerObject functions for the TASBOT, Practice Fix, Practice
# Rotation Fix, Frame Stepper, and Physics / Display FPS bypass features.
# -----------------------------------------------------------------------------

project(Bloom
    VERSION 1.0.0
    DESCRIPTION "Bloom - GD 2.1 TAS / macro bot"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR
        "Bloom targets Geometry Dash 2.1 on Windows (x86). "
        "Configure with a Windows toolchain (MSVC -A Win32, or clang-cl --target=i686-pc-windows-msvc)."
    )
endif()

# GD 2.1 is a 32-bit binary. Reject 64-bit configurations early - hooks would
# silently fail to apply and the DLL would refuse to load.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR
        "Bloom must be built as 32-bit (x86). "
        "Re-run cmake with -A Win32 (MSVC) or a 32-bit toolchain."
    )
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

include(FetchContent)

# -----------------------------------------------------------------------------
# Dependencies (vendored via FetchContent so CI is self-contained).
# -----------------------------------------------------------------------------

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        v1.3.3
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(minhook)
if(NOT minhook_POPULATED)
    FetchContent_Populate(minhook)
endif()

# MinHook 1.3.3 ships with VS / MinGW project files but no CMake build, so
# vendor the sources as a static library here.
add_library(minhook STATIC
    ${minhook_SOURCE_DIR}/src/hook.c
    ${minhook_SOURCE_DIR}/src/buffer.c
    ${minhook_SOURCE_DIR}/src/trampoline.c
    ${minhook_SOURCE_DIR}/src/hde/hde32.c
    ${minhook_SOURCE_DIR}/src/hde/hde64.c
)
target_include_directories(minhook PUBLIC ${minhook_SOURCE_DIR}/include)
set_target_properties(minhook PROPERTIES C_STANDARD 99)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# ImGui doesn't ship a CMake target by default - build it ourselves with the
# DX9 + Win32 backends. Only need these two for GD 2.1 (which uses DX9).
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_compile_definitions(imgui PUBLIC
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    IMGUI_DEFINE_MATH_OPERATORS
)

# -----------------------------------------------------------------------------
# Bloom target.
# -----------------------------------------------------------------------------

file(GLOB_RECURSE BLOOM_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.hpp
)

add_library(Bloom SHARED ${BLOOM_SOURCES})

set_target_properties(Bloom PROPERTIES
    OUTPUT_NAME "Bloom"
    PREFIX ""
)

target_include_directories(Bloom PRIVATE
    src
)

# All library names are lowercase so they resolve on both MSVC and mingw-w64.
# ImGui's DX9 backend only needs d3d9 itself - we don't pull d3dx9 in.
target_link_libraries(Bloom PRIVATE
    imgui
    minhook
    nlohmann_json::nlohmann_json
    d3d9
    dxguid
    user32
    gdi32
    shell32
    ole32
    advapi32
    version
    dwmapi
    imm32
)

target_compile_definitions(Bloom PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    BLOOM_VERSION_STRING="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(Bloom PRIVATE /W4 /permissive- /Zc:__cplusplus)
    target_link_options(Bloom PRIVATE /SAFESEH:NO)
else()
    target_compile_options(Bloom PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# Install layout (so CI can package a release artifact).
install(TARGETS Bloom RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
