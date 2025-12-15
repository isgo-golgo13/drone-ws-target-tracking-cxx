cmake_minimum_required(VERSION 3.29)
project(drone-ws-target-tracking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

#
# ============================================================================
# Boost Superproject (includes Cobalt)
# ============================================================================
#

FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(boost)

#
# ============================================================================
# nlohmann_json (header-only)
# ============================================================================
#

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

#
# ============================================================================
# fmt (required by protocol)
# ============================================================================
#

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fmt)

#
# ============================================================================
# OpenSSL (system)
# ============================================================================
#

find_package(OpenSSL REQUIRED)

message(STATUS "Using OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "Using OpenSSL libraries: ${OPENSSL_LIBRARIES}")

#
# ============================================================================
# Subdirectories
# ============================================================================
#

add_subdirectory(svckit)
add_subdirectory(protocol)
add_subdirectory(ws-server)
add_subdirectory(ws-client)

#
# ============================================================================
# Root Orchestrator Executable
# ============================================================================
#

add_executable(drone-ws-target-tracking src/main.cpp)

target_include_directories(drone-ws-target-tracking PRIVATE
    ${CMAKE_SOURCE_DIR}/ws-server/include
    ${CMAKE_SOURCE_DIR}/ws-client/include
)

target_link_libraries(drone-ws-target-tracking PRIVATE
    Boost::beast
    Boost::asio
    Boost::cobalt
    protocol-lib
    svckit
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(UNIX)
    target_link_libraries(drone-ws-target-tracking PRIVATE pthread)
endif()