#pragma once
#include <expected>
#include <string>
#include "network/nethernet/identity/ClientIdentity.hpp"
#include "network/nethernet/identity/IdentityError.hpp"
#include "core/string/Jws.hpp"

struct OfferIdentity {
    std::string strippedSdp;
    std::string token;
    std::string kid;
    std::string cpk;
    Jws fingerprint;

    std::expected<ClientIdentity, IdentityError> verify() const;
    static std::expected<OfferIdentity, IdentityError> extractOfferIdentity(std::string_view description);

private:
    static std::string reconstructFingerprint(const Jws& metadata, const std::vector<std::string_view>& fingerprints);
    static std::string toPem(const std::string& base64);
};  