#include <boost/asio.hpp>
#include <iostream>

#include "ws_client.hpp"
#include "svc_addr_config.hpp"

int main() {
    try {
        boost::asio::io_context ioc{1};

        //
        // Configure client connection + TLS validation
        //
        auto cfg = svckit::AddrConfig::FromEnvDefaults("localhost", port);

        WSClient client(ioc, cfg);
        client.start("CLIENT INITIAL MESSAGE");

        ioc.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Client failed: " << e.what() << "\n";
        return 1;
    }
}
