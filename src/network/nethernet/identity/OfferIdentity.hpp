#pragma once
#include <expected>
#include <string>
#include "network/nethernet/identity/ClientIdentity.hpp"
#include "network/nethernet/identity/IdentityError.hpp"

struct OfferIdentity {
    std::string strippedSdp;
    std::string token;
    std::string kid;

    std::expected<ClientIdentity, IdentityError> verify() const;
    static std::expected<OfferIdentity, IdentityError> extractOfferIdentity(std::string_view description);
};  