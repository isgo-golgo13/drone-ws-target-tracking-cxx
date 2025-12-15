add_executable(ws-server
  src/main.cpp
  src/ws_server.cpp
)

target_include_directories(ws-server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${boost_SOURCE_DIR}/libs/beast/include
  ${boost_SOURCE_DIR}/libs/asio/include
  ${boost_SOURCE_DIR}/libs/core/include
  ${boost_SOURCE_DIR}/libs/config/include
  ${boost_SOURCE_DIR}/libs/system/include
  ${cobalt_SOURCE_DIR}/include
)

target_link_libraries(ws-server PRIVATE
  Boost::beast
  Boost::asio
  Boost::cobalt
  protocol-lib
  svckit
  fmt::fmt
  OpenSSL::SSL
  OpenSSL::Crypto
)

if(UNIX)
  target_link_libraries(ws-server PRIVATE pthread)
endif()