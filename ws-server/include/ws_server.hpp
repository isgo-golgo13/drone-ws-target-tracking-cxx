#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/cobalt.hpp>
#include <atomic>
#include <memory>
#include <string>

#include "protocol.hpp"
#include "svc_addr_config.hpp"

class WSServer :
    public std::enable_shared_from_this<WSServer>,
    public protocol::StrategyHandler
{
public:
    WSServer(boost::asio::io_context& ioc, const svckit::AddrConfig& cfg);

    void run();
    void stop();

    // Strategy overrides
    void onUrgentRed(const protocol::Packet& p) override;
    void onNormal(const protocol::Packet& p) override;

private:
    void accept_loop();

    boost::asio::io_context& ioc_;
    std::unique_ptr<boost::asio::ssl::context> ssl_ctx_;
    boost::asio::ip::tcp::acceptor acceptor_;

    std::atomic<bool> running_{true};

    svckit::AddrConfig cfg_;
    protocol::ProtocolAPI api_;

    boost::cobalt::promise<void>
    handle_session(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream);
};
