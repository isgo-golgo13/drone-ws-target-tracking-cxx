add_library(ws-server-lib
    src/ws_server.cpp
)

target_include_directories(ws-server-lib PUBLIC include)

target_link_libraries(ws-server-lib
    PUBLIC
      svckit
      protocol-lib
      Boost::asio
      Boost::beast
      Boost::cobalt
      OpenSSL::SSL
      OpenSSL::Crypto
)

add_executable(ws-server src/main.cpp)
target_link_libraries(ws-server PRIVATE ws-server-lib)
