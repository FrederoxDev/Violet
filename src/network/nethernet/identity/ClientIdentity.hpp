#pragma once
#include <expected>
#include <string>
#include "IdentityError.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ClientIdentity {
    std::string name;
    std::string xuid;
    std::string playfabId;

    static std::expected<ClientIdentity, IdentityError> parseClientIdentity(const json& payload);
};