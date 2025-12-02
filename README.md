# Drone WebSockets Target Tracking Client/Server C++23
TLS WebSocket Server, WebSocket Client and Torpedo Firing Application Protocol using C++23, Boost.Beasts C++ WebSockets API, Boost.Cobalt Async API 


## Generating TLS Certificates for the Client and Server

```shell
mkcert -install
mkcert -cert-file certificates/server.pem -key-file certificates/server-key.pem localhost 127.0.0.1 ::1
```

