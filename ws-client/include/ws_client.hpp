#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/cobalt.hpp>

#include <string>
#include <atomic>
#include <memory>

#include "protocol.hpp"
#include "svc_addr_config.hpp"

class WSClient : public protocol::StrategyHandler
{
public:
    WSClient(boost::asio::io_context& ioc, const svckit::AddrConfig& cfg);

    void start(const std::string& initial_message);
    void stop();

    void onUrgentRed(const protocol::Packet& p) override;
    void onNormal(const protocol::Packet& p) override;

private:
    boost::asio::io_context& ioc_;
    svckit::AddrConfig cfg_;

    std::unique_ptr<boost::asio::ssl::context> ssl_ctx_;

    std::atomic<bool> running_{true};
    protocol::ProtocolAPI api_;

    boost::cobalt::task<void> run_session(std::string initial);
};