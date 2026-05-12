# Cross-compile Bloom.dll on Linux using mingw-w64 i686 (posix threads).

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR i686)

set(CMAKE_C_COMPILER   i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  i686-w64-mingw32-windres)

# Standalone DLL - no MSVC runtime dependency, link runtimes statically so the
# resulting Bloom.dll is self-contained. Adding -static also fully links
# libwinpthread-1.dll into the binary (otherwise LoadLibrary fails with Error
# 126 in environments that don't ship it next to the host EXE).
set(CMAKE_EXE_LINKER_FLAGS_INIT    "-static -static-libgcc -static-libstdc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")

set(CMAKE_FIND_ROOT_PATH "/usr/i686-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
