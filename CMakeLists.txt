cmake_minimum_required(VERSION 3.29)
project(drone-ws-target-tracking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg toolchain
include(${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)

# Add subprojects
add_subdirectory(protocol)
add_subdirectory(ws-server)
add_subdirectory(ws-client)
add_subdirectory(svckit)


# Application launcher
add_executable(drone-ws-target-tracking src/main.cpp)

target_link_libraries(drone-ws-target-tracking
    PRIVATE
      protocol-lib
      ws-server-lib
      ws-client-lib
      Boost::asio
      Boost::beast
      Boost::cobalt
)

# pthread on Unix
if(UNIX)
    target_link_libraries(drone-ws-target-tracking PRIVATE pthread)
endif()
