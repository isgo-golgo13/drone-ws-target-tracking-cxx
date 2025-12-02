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
    ssl_ctx_->load_verify_file(cfg_.ca_file);
}



void WSClient::start(const std::string& initial_message) {
    asio::co_spawn(ioc_,
        run_session(initial_message),
        asio::detached);
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
        auto results = co_await resolver.async_resolve(cfg_.host, std::to_string(cfg_.port),
                                                       boost::cobalt::use_op);


        beast::error_code ec;
        beast::ssl_stream<beast::tcp_stream> stream(ioc_, *ssl_ctx_);

        co_await beast::get_lowest_layer(stream).async_connect(
            *results.begin(),
            boost::cobalt::use_op(ec)
        );
        if (ec) co_return;

        co_await stream.async_handshake(ssl::stream_base::client,
                                        boost::cobalt::use_op(ec));
        if (ec) co_return;

        websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws(std::move(stream));

        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

        co_await ws.async_handshake(host_, "/", boost::cobalt::use_op(ec));
        if (ec) co_return;

        fmt::print("[CLIENT] Connected to {}:{}\n", host_, port_);

        // Send initial packet
        protocol::Packet pkt = api_.make_packet(initial, protocol::Urgency::GREEN);
        co_await ws.async_write(asio::buffer(pkt.payload), boost::cobalt::use_op(ec));

        // Read loop
        while (running_) {
            beast::flat_buffer buffer;

            co_await ws.async_read(buffer, boost::cobalt::use_op(ec));
            if (ec) break;

            std::string msg = beast::buffers_to_string(buffer.data());

            protocol::Packet rx = api_.make_packet(msg, protocol::Urgency::GREEN);
            api_.dispatch(rx, *this);
        }

        fmt::print("[CLIENT] Closing WS\n");
        co_await ws.async_close(websocket::close_code::normal,
                                boost::cobalt::use_op(ec));
    }
    catch (const std::exception& e) {
        std::cerr << "[CLIENT] Exception: " << e.what() << "\n";
    }
}
