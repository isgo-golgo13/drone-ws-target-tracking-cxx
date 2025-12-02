#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <cstdint>
#include <cstdlib> // for std::getenv

namespace svckit {

struct AddrConfig {
    std::string host;
    uint16_t port{0};

    std::filesystem::path cert_file;
    std::filesystem::path key_file;
    std::filesystem::path ca_file;

    std::string_view endpoint{"/"};
    std::string_view protocol_hint{"wss"};

    bool use_tls{true};

    AddrConfig() = default;

    AddrConfig(std::string h, uint16_t p,
               std::filesystem::path cert,
               std::filesystem::path key,
               std::filesystem::path ca)
        : host(std::move(h))
        , port(p)
        , cert_file(std::move(cert))
        , key_file(std::move(key))
        , ca_file(std::move(ca))
        , endpoint("/")
        , protocol_hint("wss")
        , use_tls(true) {}

    // Unified constructor that automatically derives cert paths based on env
    static AddrConfig FromEnvDefaults(std::string h, uint16_t p) {
        const char* env = std::getenv("CERT_PATH");

        std::filesystem::path base =
            (env && *env)
                ? std::filesystem::path{std::string_view(env)}
                : std::filesystem::path{"certificates"}; // fallback for local execution

        return AddrConfig{
            std::move(h),
            p,
            base / "server.pem",
            base / "server-key.pem",
            base / "server.pem"
        };
    }
};

} // namespace svckit
