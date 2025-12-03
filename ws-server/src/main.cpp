#include <boost/asio.hpp>
#include <iostream>
#include "ws_server.hpp"
#include "svc_addr_config.hpp"

int main() {
    try {
        boost::asio::io_context ioc{1};

        //
        // Configure server address + TLS
        //
        auto cfg = svckit::AddrConfig::FromEnvDefaults("0.0.0.0", port);

        WSServer server(ioc, cfg);
        server.run();

        ioc.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Server failed: " << e.what() << "\n";
        return 1;
    }
}
