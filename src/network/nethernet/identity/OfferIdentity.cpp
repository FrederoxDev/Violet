#include "OfferIdentity.hpp"
#include "MinecraftIdentityVerifier.hpp"
#include "jwt-cpp/base.h"
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <print>
#include "core/string/JsonUtils.hpp"

std::expected<OfferIdentity, IdentityError> OfferIdentity::extractOfferIdentity(std::string_view description) {
    constexpr std::string_view prefix = "a=identity:";
    const auto start = description.find(prefix);

    if (start == std::string::npos) {
        return std::unexpected(IdentityError::Unauthenticated);
    }

    size_t end = description.find("\r\n", start);
    if (end == std::string::npos) {
        end = description.size();
    }
    else {
        end += 2; // include "\r\n"
    }

    std::string res;
    res.reserve(description.size());
    res.append(description.substr(0, start));
    res.append(description.substr(end));

    std::string jwtbase64 = std::string(description.substr(start + prefix.size(), end - (start + prefix.size()) - 2)); // exclude "\r\n"

    std::string identityStr;
    try {
        identityStr = jwt::base::decode<jwt::alphabet::base64>(jwtbase64);
    }
    catch (const std::exception& e) {
        std::println("Failed to decode JWT: {}", e.what());
        return std::unexpected(IdentityError::InvalidJWT);
    }

    auto identity = JsonUtils::parseObject(identityStr);
    if (!identity) return std::unexpected(IdentityError::InvalidJWT);

    auto assertionJson = JsonUtils::getStringifiedObject(*identity, "assertion");
    if (!assertionJson) return std::unexpected(IdentityError::InvalidJWT);

    auto tokenStr = JsonUtils::getString(*assertionJson, "token");
    if (!tokenStr) return std::unexpected(IdentityError::InvalidJWT);

    auto decoded = jwt::decode<jwt::traits::nlohmann_json>(*tokenStr);
    auto kid = decoded.get_header_claim("kid").as_string();

    return OfferIdentity{
        .strippedSdp = std::move(res),
        .token = *tokenStr,
        .kid = std::move(kid)
    };
}

std::expected<ClientIdentity, IdentityError> OfferIdentity::verify() const {
    try {
        auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);
        auto kid = decoded.get_header_claim("kid").as_string();
        auto payloadStr = decoded.get_payload();
        auto payload = JsonUtils::parseObject(payloadStr);
        if (!payload) return std::unexpected(IdentityError::InvalidJWT);

        auto result = MinecraftIdentityVerifier::verifyIdentity(kid, token);
        if (result) {
            return ClientIdentity::parseClientIdentity(*payload);
        }

        return std::unexpected(IdentityError::Unauthenticated);
    }
    catch (const std::exception& e) {
        std::println("Failed to decode JWT: {}", e.what());
        return std::unexpected(IdentityError::InvalidJWT);
    }
}