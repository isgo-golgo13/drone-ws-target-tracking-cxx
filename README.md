# Drone WebSockets Target Tracking Client/Server C++23
TLS WebSocket Server, WebSocket Client and Torpedo Firing Application Protocol using C++23, Boost.Beasts C++ WebSockets API, Boost.Cobalt Async API 


## Overview

drone-ws-target-tracking is a full-stack, modern C++23 networking system built to show:

- Asynchronous, coroutine-based TLS WebSocket communication
- Modular and reusable address/configuration framework `(svckit::AddrConfig)`
- Shared binary protocol library with bitfield headers `(protocol/)`
- A TLS-enabled WebSocket server `(ws-server/)`
- A TLS-enabled WebSocket client (ws-client/)
- A root-level orchestrator application `(src/main.cpp)`
- Full Dockerized development and deployment pipeline
- Automatic certificate-path resolution using environment variables `(CERT_PATH)`
- Separation of Concerns **(SoC)** suitable for enterprise or gaming services integration
- A future-proof pathway for QUIC (HTTP/3) integration

This source code is written using C++23 and anchored in high-performance API stacks:

- Boost.Beast (TLS WebSocket transport)
- Boost.Cobalt (C++ coroutines)
- Boost.Asio (networking)
- OpenSSL (TLS)
- CMake and **vcpkg** (dependency management)
- Multi-stage rootless Docker builds


## Project Repository Structure

```shell
drone-ws-target-tracking-cxx
├── CMakeLists.txt
├── Dockerfile
├── README.md
├── certificates
├── cmake
│   └── toolchain.cmake
├── docker-compose.yaml
├── protocol
│   ├── CMakeLists.txt
│   ├── include
│   │   └── protocol.hpp
│   └── src
│       └── protocol.cpp
├── src
│   └── main.cpp
├── svckit
│   ├── CMakeLists.txt
│   └── include
│       └── svc_addr_config.hpp
├── vcpkg.json
├── ws-client
│   ├── CMakeLists.txt
│   ├── Dockerfile
│   ├── include
│   │   └── ws_client.hpp
│   └── src
│       ├── main.cpp
│       └── ws_client.cpp
└── ws-server
    ├── CMakeLists.txt
    ├── Dockerfile
    ├── include
    │   └── ws_server.hpp
    └── src
        ├── main.cpp
        └── ws_server.cpp
```




## Generating TLS Certificates for the Client and Server

```shell
mkcert -install
mkcert -cert-file certificates/server.pem -key-file certificates/server-key.pem localhost 127.0.0.1 ::1
```

