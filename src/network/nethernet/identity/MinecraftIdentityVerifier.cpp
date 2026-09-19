#include "MinecraftIdentityVerifier.hpp"
#include <future>
#include <httplib.h>
#include <mutex>
#include <print>
#include <string>
#include <system_error>
#include <unordered_map>
#include "core/string/JsonUtils.hpp"
#include "jwt-cpp/base.h"
#include "jwt-cpp/jwt.h"
#include <jwt-cpp/traits/nlohmann-json/traits.h>

std::mutex MinecraftIdentityVerifier::mRefreshMutex;
std::mutex MinecraftIdentityVerifier::mKeysMutex;
std::shared_future<bool> MinecraftIdentityVerifier::mKeysRefreshFuture;
std::unordered_map<std::string, std::string> MinecraftIdentityVerifier::mKidToPemMap;

bool MinecraftIdentityVerifier::refreshKeys() {
    httplib::SSLClient client("authorization.franchise.minecraft-services.net");
    auto res = client.Get("/.well-known/keys");

    if (!res) {
        std::println("JWKS fetch failed: {}", httplib::to_string(res.error()));
        return false;
    }

    if (res->status != 200) {
        std::println("JWKS fetch failed with status: {}", res->status);
        return false;
    }

    auto data = JsonUtils::parseObject(res->body); 
    if (!data) return false;

    auto keys = JsonUtils::getObjectArray(*data, "keys");
    if (!keys) return false;

    std::unordered_map<std::string, std::string> kidToPemMap;

    for (const auto& key : *keys) {
        auto kid = JsonUtils::getString(key, "kid");
        auto n = JsonUtils::getString(key, "n");
        auto e = JsonUtils::getString(key, "e");
        if (!kid || !n || !e) continue;

        try {
            std::error_code ec;
            auto pem = jwt::helper::create_public_key_from_rsa_components(*n, *e, [](const std::string& str) {
                return jwt::base::decode<jwt::alphabet::base64url>(
                    jwt::base::pad<jwt::alphabet::base64url>(str)
                );
            }, ec);

            if (ec) {
                std::println("Error creating public key for kid {}: {}", *kid, ec.message());
                continue;
            }
            kidToPemMap.emplace(*kid, std::move(pem));
        }
        catch (const std::exception& err) {
            std::println("Exception while creating public key for kid {}: {}", *kid, err.what());
            continue;
        }
    }

    if (kidToPemMap.empty()) return false;
    {
        std::lock_guard<std::mutex> lock(mKeysMutex);
        mKidToPemMap = std::move(kidToPemMap);
    }

    return true;
}

std::shared_future<bool> MinecraftIdentityVerifier::refreshKeysAsync() {
    std::lock_guard<std::mutex> lock(mRefreshMutex);

    const bool inFlight = 
        mKeysRefreshFuture.valid() &&
        mKeysRefreshFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready;

    if (!inFlight) {
        mKeysRefreshFuture = std::async(std::launch::async, []() {
            return MinecraftIdentityVerifier::refreshKeys();
        }).share();
    }

    return mKeysRefreshFuture;
}

bool MinecraftIdentityVerifier::verifyIdentity(const std::string& kid, const std::string& token) {
    auto findKey = [](const std::string& kid) -> std::optional<std::string> {
        std::lock_guard<std::mutex> lock(mKeysMutex);
        auto it = mKidToPemMap.find(kid);
        if (it == mKidToPemMap.end()) return std::nullopt;
        return it->second;
    };

    auto pem = findKey(kid);
    if (!pem) {
        auto future = refreshKeysAsync();
        if (future.wait_for(std::chrono::seconds(3)) != std::future_status::ready) {
            std::println("Key refresh timed out");
            return false;
        }

        if (!future.get()) return false;
        pem = findKey(kid);
    }

    if (!pem) {
        std::println("No public key found for kid: {}", kid);
        return false;
    }

    auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
        .with_issuer("https://authorization.franchise.minecraft-services.net/")
        .with_audience("api://auth-minecraft-services/multiplayer")
        .allow_algorithm(jwt::algorithm::rs256(*pem, "", "", ""));

    std::error_code ec;
    try {
        verifier.verify(jwt::decode<jwt::traits::nlohmann_json>(token), ec);
    }
    catch (const std::exception& e) {
        std::println("JWT verification exception: {}", e.what());
        return false;
    }

    if (ec) {
        std::println("JWT verification failed: {}", ec.message());
        return false;
    }

    return true;
}