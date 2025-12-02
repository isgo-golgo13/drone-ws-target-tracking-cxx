#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <nlohmann/json.hpp>

namespace protocol {

enum class Urgency : uint8_t {
    GREEN = 0,
    YELLOW = 1,
    RED = 2
};

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t version : 4;
    uint16_t type    : 4;
    uint16_t urgent  : 2;
    uint16_t reserved: 6;
    uint32_t length;
};
#pragma pack(pop)

struct Packet {
    PacketHeader header{};
    std::vector<uint8_t> payload;
};

class StrategyHandler {
public:
    virtual ~StrategyHandler() = default;
    virtual void onUrgentRed(const Packet& p) = 0;
    virtual void onNormal(const Packet& p) = 0;
};

class ProtocolAPI {
public:
    ProtocolAPI() = default;

    Packet make_packet(const std::string& msg, Urgency u);

    void dispatch(const Packet& p, StrategyHandler& handler);

    static std::string packet_to_json(const Packet& p) {
        nlohmann::json j;
        j["version"] = p.header.version;
        j["type"] = p.header.type;
        j["urgent"] = p.header.urgent;
        j["length"] = p.header.length;
        j["payload"] = std::string(p.payload.begin(), p.payload.end());
        return j.dump();
    }
};

} // namespace protocol
