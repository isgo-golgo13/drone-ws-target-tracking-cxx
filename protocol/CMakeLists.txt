add_library(protocol-lib
    src/protocol.cpp
)

target_include_directories(protocol-lib PUBLIC include)

target_link_libraries(protocol-lib PRIVATE fmt nlohmann_json::nlohmann_json)
