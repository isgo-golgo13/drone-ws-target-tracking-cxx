set(CMAKE_SYSTEM_NAME Darwin)

set(CMAKE_C_COMPILER /opt/homebrew/bin/gcc-15)
set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-15)
set(CMAKE_ASM_COMPILER /opt/homebrew/bin/gcc-15)

# Prevent AppleClang sysroot injection
set(CMAKE_OSX_SYSROOT "" CACHE PATH "" FORCE)
