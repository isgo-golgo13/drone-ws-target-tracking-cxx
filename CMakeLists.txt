cmake_minimum_required(VERSION 3.29)
project(drone-ws-target-tracking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

#
# ============================================================================
# Boost Superproject (SHALLOW clone, FAST)
# ============================================================================
#

FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

# Only pull necessary submodules for our use-case:
set(FETCHCONTENT_SOURCE_DIR_BOOST ${boost_SOURCE_DIR})

# Disable building docs, tests, and examples
set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)
set(BOOST_INCLUDE_LIBRARIES
    asio
    beast
    system
    core
    config
    callable_traits
    align
    assert
    static_assert
    type_traits
    throw_exception
    smart_ptr
    mp11
)

FetchContent_MakeAvailable(boost)

#
# ============================================================================
# Boost.Cobalt (must be fetched separately)
# ============================================================================
#

FetchContent_Declare(
  cobalt
  GIT_REPOSITORY https://github.com/boostorg/cobalt.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cobalt)

# Cobalt library target is defined by its own CMake
# (target name: boost_cobalt)

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
    ${boost_SOURCE_DIR}
    ${boost_SOURCE_DIR}/asio/include
    ${boost_SOURCE_DIR}/beast/include
    ${boost_SOURCE_DIR}/system/include
    ${boost_SOURCE_DIR}/core/include
    ${boost_SOURCE_DIR}/config/include
    ${cobalt_SOURCE_DIR}/include
)

target_link_libraries(drone-ws-target-tracking PRIVATE
    protocol-lib
    svckit
    boost_cobalt
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(UNIX)
    target_link_libraries(drone-ws-target-tracking PRIVATE pthread)
endif()
