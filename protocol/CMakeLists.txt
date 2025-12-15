add_library(protocol-lib
    src/protocol.cpp
)

target_include_directories(protocol-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${nlohmann_json_SOURCE_DIR}/include
)

target_link_libraries(protocol-lib
    PUBLIC
      nlohmann_json::nlohmann_json
      fmt::fmt
)
