add_executable(ws-server
  src/main.cpp
  src/ws_server.cpp
)

target_include_directories(ws-server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${boost_SOURCE_DIR}
  ${boost_SOURCE_DIR}/asio/include
  ${boost_SOURCE_DIR}/beast/include
  ${boost_cobalt_SOURCE_DIR}/include
)

target_link_libraries(ws-server PRIVATE
  protocol-lib
  svckit
  boost_cobalt
  OpenSSL::SSL
  OpenSSL::Crypto
)

if(UNIX)
  target_link_libraries(ws-server PRIVATE pthread)
endif()
