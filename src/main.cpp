#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <stop_token>
#include <memory>

#include "ws_server.hpp"
#include "ws_client.hpp"

static std::atomic<bool> g_running{true};

void sig_handler(int) {
    g_running = false;
}

class Application {
public:
    Application() = default;
    ~Application() = default;

    void run() {
        std::signal(SIGINT, sig_handler);

        std::jthread server_thread([this](std::stop_token st){
            run_server(st);
        });

        std::jthread client_thread([this](std::stop_token st){
            run_client(st);
        });

        while (g_running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Shutting down...\n";
    }

private:
    void run_server(std::stop_token st) {
        try {
            boost::asio::io_context ioc{1};
             //WSClient client (ioc, addrConfjg) - AddrConfig (std::string_view : "localhost", short: port, std::filesystem::path : certs)
            WSServer server(ioc, 8443, "certificates/server.pem", "certificates/server-key.pem");
            server.run();

            while (!st.stop_requested() && g_running.load()) {
                ioc.run_for(std::chrono::milliseconds(50));
            }

            server.stop();

        } catch (const std::exception& e) {
            std::cerr << "Server exception: " << e.what() << "\n";
        }
    }

    void run_client(std::stop_token st) {
        try {
            boost::asio::io_context ioc{1};
            //WSClient client (ioc, addrConfjg) - AddrConfig (std::string_view : "localhost", short: port, std::filesystem::path : certs)
            WSClient client(ioc, "localhost", "8443", "certificates/server.pem");
            client.start("HELLO FROM CLIENT");

            while (!st.stop_requested() && g_running.load()) {
                ioc.run_for(std::chrono::milliseconds(50));
            }

            client.stop();

        } catch (const std::exception& e) {
            std::cerr << "Client exception: " << e.what() << "\n";
        }
    }
};

int main() {
    Application app;
    app.run();
    return 0;
}
