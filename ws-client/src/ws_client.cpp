#include "ws_client.hpp"
#include <fmt/core.h>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

WSClient::WSClient(boost::asio::io_context& ioc,
                   const svckit::AddrConfig& cfg)
    : ioc_(ioc)
    , cfg_(cfg)
    , ssl_ctx_(std::make_unique<ssl::context>(ssl::context::tlsv12_client))
{
    ssl_ctx_->set_verify_mode(ssl::verify_peer);
    ssl_ctx_->load_verify_file(cfg_.ca_file.string());
}

void WSClient::start(const std::string& initial_message) {
    boost::cobalt::spawn(ioc_, run_session(initial_message), asio::detached);
}

void WSClient::stop() {
    running_ = false;
}

//
// Strategy
//

void WSClient::onNormal(const protocol::Packet& p) {
    fmt::print("[CLIENT] Normal response from server: {}\n",
               std::string(p.payload.begin(), p.payload.end()));
}

void WSClient::onUrgentRed(const protocol::Packet& p) {
    fmt::print("[CLIENT] RED ALERT: Drone target received.\n");
    auto s = std::string(p.payload.begin(), p.payload.end());
    fmt::print("[CLIENT] Payload: {}\n", s);
}

//
// Main coroutine session
//

boost::cobalt::task<void> WSClient::run_session(std::string initial) {
    try {
        tcp::resolver resolver(ioc_);
        auto results = co_await resolver.async_resolve(
            cfg_.host, 
            std::to_string(cfg_.port),
            boost::cobalt::use_op);

        // Create SSL stream over TCP socket
        ssl::stream<tcp::socket> ssl_stream(ioc_, *ssl_ctx_);

        // Connect TCP
        co_await beast::get_lowest_layer(ssl_stream).async_connect(
            *results.begin(),
            boost::cobalt::use_op
        );

        // SSL handshake
        co_await ssl_stream.async_handshake(ssl::stream_base::client,
                                            boost::cobalt::use_op);

        // WebSocket stream over SSL
        websocket::stream<ssl::stream<tcp::socket>> ws(std::move(ssl_stream));

        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        co_await ws.async_handshake(cfg_.host, "/", boost::cobalt::use_op);

        fmt::print("[CLIENT] Connected to {}:{}\n", cfg_.host, cfg_.port);

        // Send initial packet
        protocol::Packet pkt = api_.make_packet(initial, protocol::Urgency::GREEN);
        co_await ws.async_write(asio::buffer(pkt.payload), boost::cobalt::use_op);

        // Read loop
        while (running_) {
            beast::flat_buffer buffer;

            auto [ec, bytes] = co_await ws.async_read(buffer, 
                asio::as_tuple(boost::cobalt::use_op));
            
            if (ec) {
                if (ec != websocket::error::closed)
                    fmt::print("[CLIENT] Read error: {}\n", ec.message());
                break;
            }

            std::string msg = beast::buffers_to_string(buffer.data());
            protocol::Packet rx = api_.make_packet(msg, protocol::Urgency::GREEN);
            api_.dispatch(rx, *this);
        }

        fmt::print("[CLIENT] Closing WS\n");
        co_await ws.async_close(websocket::close_code::normal,
                                asio::as_tuple(boost::cobalt::use_op));
    }
    catch (const std::exception& e) {
        std::cerr << "[CLIENT] Exception: " << e.what() << "\n";
    }
}