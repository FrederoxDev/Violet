#include "OfferIdentity.hpp"
#include "MinecraftIdentityVerifier.hpp"
#include "jwt-cpp/base.h"
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <print>
#include "core/string/JsonUtils.hpp"
#include "core/string/Jws.hpp"
#include "core/crypto/SdpUtils.hpp"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

std::expected<OfferIdentity, IdentityError> OfferIdentity::extractOfferIdentity(std::string_view description) {
    auto jwtBase64 = SdpUtils::extract(description, "a=identity:");
    if (!jwtBase64) return std::unexpected(IdentityError::MissingIdentity);

    std::string strippedDescription = SdpUtils::strip(description, "a=identity:");

    std::string identityStr;
    try {
        identityStr = jwt::base::decode<jwt::alphabet::base64>(std::string(*jwtBase64));
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

    auto fingerprintsStr = JsonUtils::getString(*assertionJson, "fingerprints");
    if (!fingerprintsStr) return std::unexpected(IdentityError::InvalidJWT);

    auto fingerprintsMetadata = Jws::parse(*fingerprintsStr);
    auto fingerprints = SdpUtils::extractAll(description, "a=fingerprint:");
    if (!fingerprintsMetadata || fingerprints.empty()) {
        std::println("Missing fingerprints metadata or no fingerprints found");
        return std::unexpected(IdentityError::InvalidJWT);
    }
    
    auto decoded = jwt::decode<jwt::traits::nlohmann_json>(*tokenStr);
    auto kid = decoded.get_header_claim("kid").as_string();
    auto payloadStr = decoded.get_payload();
    auto payload = JsonUtils::parseObject(payloadStr);
    if (!payload) return std::unexpected(IdentityError::InvalidJWT);

    auto cpk = JsonUtils::getString(*payload, "cpk");
    if (!cpk) return std::unexpected(IdentityError::InvalidJWT);

    std::string reconstructedFingerprint = reconstructFingerprint(*fingerprintsMetadata, fingerprints);
    fingerprintsMetadata->payload = reconstructedFingerprint;

    return OfferIdentity{
        .strippedSdp = std::move(strippedDescription),
        .token = *tokenStr,
        .kid = std::move(kid),
        .cpk = *cpk,
        .fingerprint = *fingerprintsMetadata
    };
}

std::expected<ClientIdentity, IdentityError> OfferIdentity::verify() const {
    try {
        auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);
        auto kid = decoded.get_header_claim("kid").as_string();
        auto payloadStr = decoded.get_payload();
        auto payload = JsonUtils::parseObject(payloadStr);
        if (!payload) return std::unexpected(IdentityError::InvalidJWT);

        const bool mojangSigned = MinecraftIdentityVerifier::verifyIdentity(kid, token);
        if (!mojangSigned) {
            return std::unexpected(IdentityError::Unauthenticated);
        }

        std::string cpkPem = OfferIdentity::toPem(cpk);

        auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
            .allow_algorithm(jwt::algorithm::es384(cpkPem, "", "", ""));

        std::error_code ec;

        try {
            verifier.verify(jwt::decode<jwt::traits::nlohmann_json>(fingerprint.toString()), ec);
        }
        catch (const std::exception& e) {
            std::println("JWT verification failed err: {}", e.what());
            return std::unexpected(IdentityError::InvalidFingerprint);
        }

        if (ec) {
            std::println("JWT verification failed: {}", ec.message());
            return std::unexpected(IdentityError::InvalidFingerprint);
        }

        return ClientIdentity::parseClientIdentity(*payload);
    }
    catch (const std::exception& e) {
        std::println("Failed to decode JWT: {}", e.what());
        return std::unexpected(IdentityError::InvalidJWT);
    }
}

std::string OfferIdentity::reconstructFingerprint(const Jws& metadata, const std::vector<std::string_view>& rawFingerprints) {
    json fingerprint = json::array();

    for (const auto& rawFingerprint : rawFingerprints) {
        auto firstSpace = rawFingerprint.find(' ');
        if (firstSpace == std::string_view::npos) {
            std::println("Invalid fingerprint format: {}", rawFingerprint);
            continue;
        }

        json entry = {
            {"algorithm", std::string(rawFingerprint.substr(0, firstSpace))},
            {"digest", std::string(rawFingerprint.substr(firstSpace + 1))}
        };

        fingerprint.push_back(entry);
    }

    json result = {
        {"fingerprint", fingerprint}
    };

    return result.dump();
}

std::string OfferIdentity::toPem(const std::string& base64) {
    std::string pem = "-----BEGIN PUBLIC KEY-----\n";
    for (size_t i = 0; i < base64.size(); i += 64) {
        pem += base64.substr(i, 64) + "\n";
    }  
    pem += "-----END PUBLIC KEY-----\n";
    return pem;
}