cmake_minimum_required(VERSION 3.29)
project(drone-ws-target-tracking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

#
# ============================================================================
# Fetch Boost Superproject (Beast, Asio, Cobalt, System, etc.)
# ============================================================================
#

FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.85.0
)

set(BOOST_INCLUDE_LIBRARIES
    asio
    beast
    system
    core
    config
    function
    smart_ptr
    throw_exception
)

# Cobalt is not an official Boost release dependency yet,
# so we fetch it independently below.
set(BOOST_EXCLUDE_LIBRARIES "")

FetchContent_MakeAvailable(boost)

#
# ============================================================================
# Fetch Boost.Cobalt (coroutines)
# ============================================================================
#
FetchContent_Declare(
  boost_cobalt
  GIT_REPOSITORY https://github.com/boostorg/cobalt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(boost_cobalt)
add_library(boost_cobalt STATIC
  ${boost_cobalt_SOURCE_DIR}/src/awaitables.cpp
  ${boost_cobalt_SOURCE_DIR}/src/future.cpp
)

target_include_directories(boost_cobalt PUBLIC
  ${boost_cobalt_SOURCE_DIR}/include
)

#
# ============================================================================
# Fetch nlohmann_json (header-only)
# ============================================================================
#
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

#
# ============================================================================
# Find OpenSSL
# ============================================================================
#
find_package(OpenSSL REQUIRED)

message(STATUS "Using OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "Using OpenSSL libraries: ${OPENSSL_LIBRARIES}")

#
# ============================================================================
# Subdirectories (libraries and executables)
# ============================================================================
#

add_subdirectory(svckit)
add_subdirectory(protocol)
add_subdirectory(ws-server)
add_subdirectory(ws-client)

#
# ============================================================================
# Root application
# ============================================================================
#
add_executable(drone-ws-target-tracking src/main.cpp)

target_include_directories(drone-ws-target-tracking PRIVATE
  ${boost_SOURCE_DIR}
  ${boost_SOURCE_DIR}/asio/include
  ${boost_SOURCE_DIR}/beast/include
  ${boost_cobalt_SOURCE_DIR}/include
)

target_link_libraries(drone-ws-target-tracking PRIVATE
  protocol-lib
  svckit
  OpenSSL::SSL
  OpenSSL::Crypto
  boost_cobalt
)

if(UNIX)
  target_link_libraries(drone-ws-target-tracking PRIVATE pthread)
endif()
