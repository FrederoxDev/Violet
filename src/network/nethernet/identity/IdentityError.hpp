#pragma once

enum class IdentityError {
    InvalidJWT,
    Unauthenticated,
    MissingIdentity,
    InvalidFingerprint,
};