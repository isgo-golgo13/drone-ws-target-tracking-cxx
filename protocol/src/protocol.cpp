#include "protocol.hpp"

namespace protocol {

Packet ProtocolAPI::make_packet(const std::string& msg, Urgency u) {
    Packet pkt;
    pkt.header.version = 1;
    pkt.header.type = 1;
    pkt.header.urgent = static_cast<uint16_t>(u);
    pkt.header.reserved = 0;
    pkt.header.length = msg.size();
    pkt.payload.assign(msg.begin(), msg.end());
    return pkt;
}

void ProtocolAPI::dispatch(const Packet& p, StrategyHandler& handler) {
    if (static_cast<Urgency>(p.header.urgent) == Urgency::RED) {
        handler.onUrgentRed(p);
        return;
    }
    handler.onNormal(p);
}

} // namespace protocol
