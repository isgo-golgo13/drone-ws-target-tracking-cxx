#include "ws_server.hpp"
#include <fmt/core.h>
#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

WSServer::WSServer(boost::asio::io_context& ioc,
                   const svckit::AddrConfig& cfg)
    : ioc_(ioc)
    , ssl_ctx_(std::make_unique<ssl::context>(ssl::context::tlsv12_server))
    , acceptor_(ioc)
    , cfg_(cfg)
{
    ssl_ctx_->set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);

    ssl_ctx_->use_certificate_file(cfg.cert_file, ssl::context::pem);
    ssl_ctx_->use_private_key_file(cfg.key_file, ssl::context::pem);

    tcp::endpoint ep{tcp::v4(), cfg_.port};
    acceptor_.open(ep.protocol());
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(ep);
    acceptor_.listen();
}


void WSServer::run() {
    std::cout << "Server listening on 0.0.0.0:" << port_ << "\n";
    accept_loop();
}

void WSServer::stop() {
    running_ = false;
    beast::error_code ec;
    acceptor_.close(ec);
}

void WSServer::accept_loop() {
    asio::co_spawn(ioc_,
        [this]() -> boost::cobalt::task<void> {
            while (running_) {
                beast::error_code ec;
                tcp::socket socket(ioc_);
                acceptor_.accept(socket, ec);
                if (ec) {
                    co_await boost::cobalt::sleep_for(std::chrono::milliseconds(50));
                    continue;
                }

                auto stream = ssl::stream<tcp::socket>(std::move(socket), *ssl_ctx_);
                asio::co_spawn(ioc_,
                    handle_session(std::move(stream)),
                    asio::detached);
            }
        },
        asio::detached);
}

boost::cobalt::promise<void> WSServer::handle_session(
    ssl::stream<tcp::socket> stream)
{
    beast::error_code ec;
    co_await asio::async_handshake(stream, ssl::stream_base::server,
                                   boost::cobalt::use_op(ec));

    if (ec) co_return;

    websocket::stream<ssl::stream<tcp::socket>> ws(std::move(stream));

    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    co_await ws.async_accept(boost::cobalt::use_op(ec));
    if (ec) co_return;

    std::cout << "[SERVER] WebSocket session opened\n";

    while (running_) {
        beast::flat_buffer buffer;

        co_await ws.async_read(buffer, boost::cobalt::use_op(ec));
        if (ec) break;

        std::string msg = beast::buffers_to_string(buffer.data());

        protocol::Packet pkt = api_.make_packet(msg, protocol::Urgency::GREEN);

        api_.dispatch(pkt, *this);

        co_await ws.async_write(asio::buffer(msg), boost::cobalt::use_op(ec));
        if (ec) break;
    }

    std::cout << "[SERVER] WebSocket session closed\n";
}

//
// Strategy dispatches
//

void WSServer::onNormal(const protocol::Packet& p) {
    fmt::print("[SERVER] Normal packet: {}\n",
               std::string(p.payload.begin(), p.payload.end()));
}

void WSServer::onUrgentRed(const protocol::Packet& p) {
    fmt::print("[SERVER] URGENT RED — STREAMING DRONE TARGET DATA\n");

    // SSE-like stream simulation
    std::thread([p](){
        for (int i = 0; i < 5; ++i) {
            fmt::print("[DRONE STREAM] lat={}, lon={}\n",
                       34.2345 + i * 0.0001,
                       69.1234 + i * 0.0002);
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
    }).detach();
}
