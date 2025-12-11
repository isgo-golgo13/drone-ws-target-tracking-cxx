add_executable(ws-client
  src/main.cpp
  src/ws_client.cpp
)

target_include_directories(ws-client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${boost_SOURCE_DIR}
  ${boost_SOURCE_DIR}/asio/include
  ${boost_SOURCE_DIR}/beast/include
  ${boost_cobalt_SOURCE_DIR}/include
)

target_link_libraries(ws-client PRIVATE
  protocol-lib
  svckit
  boost_cobalt
  OpenSSL::SSL
  OpenSSL::Crypto
)

if(UNIX)
  target_link_libraries(ws-client PRIVATE pthread)
endif()
