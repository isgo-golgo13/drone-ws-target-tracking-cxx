cmake_minimum_required(VERSION 3.29)
project(drone-ws-target-tracking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vcpkg toolchain
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain automatically.")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Subdirectories
add_subdirectory(protocol)
add_subdirectory(ws-server)
add_subdirectory(ws-client)

# Root executable (application launcher)
add_executable(drone-ws-target-tracking src/main.cpp)

target_link_libraries(drone-ws-target-tracking
    PRIVATE
        protocol-lib
        ws-server-lib
        ws-client-lib
)

# Enable -pthread
if(APPLE)
    target_link_options(drone-ws-target-tracking PRIVATE "-pthread")
else()
    target_link_libraries(drone-ws-target-tracking PRIVATE pthread)
endif()
