add_library(ws-client-lib
    src/ws_client.cpp
)

target_include_directories(ws-client-lib PUBLIC include)

target_link_libraries(ws-client-lib
    PUBLIC
      svckit
      protocol-lib
      Boost::asio
      Boost::beast
      Boost::cobalt
      OpenSSL::SSL
      OpenSSL::Crypto
)

add_executable(ws-client src/main.cpp)
target_link_libraries(ws-client PRIVATE ws-client-lib)
