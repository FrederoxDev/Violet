#pragma once
#include <string>
#include <expected>
#include <print>
#include "core/string/Base64.hpp"

class ClientIdentity {
public:
    enum class IdentityError {
        Unauthenticated,
        InvalidJWT
    };

    struct ClientIdentityData {
        std::string strippedDescription;
    };

    static std::expected<ClientIdentityData, IdentityError> extractClientIdentity(std::string_view description) {
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
        auto jwtRes = Base64::decodeBase64(jwtbase64);

        if (!jwtRes.has_value()) {
            return std::unexpected(IdentityError::InvalidJWT);
        }

        auto jwt = jwtRes.value();
        std::println("Extracted JWT: {}", jwt);

        return ClientIdentityData{res};
    }
};