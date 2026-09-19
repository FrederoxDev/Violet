#include <expected>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include "ClientIdentity.hpp"
#include "core/string/JsonUtils.hpp"

std::expected<ClientIdentity, IdentityError> ClientIdentity::parseClientIdentity(const json& payload) {
    auto name = JsonUtils::getString(payload, "xname");
    auto xuid = JsonUtils::getString(payload, "xid");
    auto playfabId = JsonUtils::getString(payload, "mid");

    if (!name || !xuid || !playfabId) {
        return std::unexpected(IdentityError::InvalidJWT);
    }

    ClientIdentity identity;
    identity.name = *name;
    identity.xuid = *xuid;
    identity.playfabId = *playfabId;
    return identity;
}