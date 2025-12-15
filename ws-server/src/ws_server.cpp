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

    ssl_ctx_->use_certificate_file(cfg_.cert_file.string(), ssl::context::pem);
    ssl_ctx_->use_private_key_file(cfg_.key_file.string(), ssl::context::pem);

    tcp::endpoint ep{tcp::v4(), cfg_.port};
    acceptor_.open(ep.protocol());
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(ep);
    acceptor_.listen();
}

void WSServer::run() {
    fmt::print("Server listening on 0.0.0.0:{}\n", cfg_.port);
    boost::cobalt::spawn(ioc_, accept_loop(), asio::detached);
}

void WSServer::stop() {
    running_ = false;
    beast::error_code ec;
    acceptor_.close(ec);
}

boost::cobalt::task<void> WSServer::accept_loop() {
    while (running_) {
        auto [ec, socket] = co_await acceptor_.async_accept(
            asio::as_tuple(boost::cobalt::use_op));
        
        if (ec) {
            if (running_)
                fmt::print("[SERVER] Accept error: {}\n", ec.message());
            continue;
        }

        boost::cobalt::spawn(ioc_, handle_session(std::move(socket)), asio::detached);
    }
}

boost::cobalt::task<void> WSServer::handle_session(tcp::socket socket) {
    try {
        // Create SSL stream
        ssl::stream<tcp::socket> ssl_stream(std::move(socket), *ssl_ctx_);

        // SSL handshake
        co_await ssl_stream.async_handshake(ssl::stream_base::server,
                                            boost::cobalt::use_op);

        // WebSocket stream over SSL
        websocket::stream<ssl::stream<tcp::socket>> ws(std::move(ssl_stream));

        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

        co_await ws.async_accept(boost::cobalt::use_op);

        fmt::print("[SERVER] WebSocket session opened\n");

        while (running_) {
            beast::flat_buffer buffer;

            auto [ec, bytes] = co_await ws.async_read(buffer,
                asio::as_tuple(boost::cobalt::use_op));

            if (ec) {
                if (ec != websocket::error::closed)
                    fmt::print("[SERVER] Read error: {}\n", ec.message());
                break;
            }

            std::string msg = beast::buffers_to_string(buffer.data());

            protocol::Packet pkt = api_.make_packet(msg, protocol::Urgency::GREEN);
            api_.dispatch(pkt, *this);

            co_await ws.async_write(asio::buffer(msg), boost::cobalt::use_op);
        }

        fmt::print("[SERVER] WebSocket session closed\n");
    }
    catch (const std::exception& e) {
        fmt::print("[SERVER] Session exception: {}\n", e.what());
    }
}

//
// Strategy dispatches
//

void WSServer::onNormal(const protocol::Packet& p) {
    fmt::print("[SERVER] Normal packet: {}\n",
               std::string(p.payload.begin(), p.payload.end()));
}

void WSServer::onUrgentRed(const protocol::Packet& p) {
    fmt::print("[SERVER] URGENT RED - STREAMING DRONE TARGET DATA\n");

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