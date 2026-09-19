#pragma once
#include <string>
#include <expected>
#include <jwt-cpp/base.h>
#include <print>
#include "core/string/base64_nopad.hpp"

class Jws {
public:
    std::string header;
    std::string payload;
    std::string signature;

    enum class ParseError {
        InvalidFormat,
        Base64DecodeError,
        WrongSegmentsCount,
    };

    static std::expected<Jws, ParseError> parse(std::string_view jws) {
        auto firstDot = jws.find('.');
        if (firstDot == std::string_view::npos) {
            return std::unexpected(ParseError::InvalidFormat);
        }

        auto secondDot = jws.find('.', firstDot + 1);
        if (secondDot == std::string_view::npos) {
            return std::unexpected(ParseError::InvalidFormat);
        }

        auto thirdDot = jws.find('.', secondDot + 1);
        if (thirdDot != std::string_view::npos) {
            return std::unexpected(ParseError::WrongSegmentsCount);
        }

        auto parseSegment = [](std::string_view segment) -> std::string {
            return jwt::base::decode<jwt::alphabet::base64url>(
                jwt::base::pad<jwt::alphabet::base64url>(std::string(segment))
            );
        };

        Jws result;
        try {
            result.header = parseSegment(jws.substr(0, firstDot));
            result.payload = parseSegment(jws.substr(firstDot + 1, secondDot - firstDot - 1));
            result.signature = parseSegment(jws.substr(secondDot + 1));
        } catch (const std::exception& e) {
            return std::unexpected(ParseError::Base64DecodeError);
        }

        return result;
    }

    Jws() = default;

    Jws(std::string header, std::string payload, std::string signature)
        : header(std::move(header)), payload(std::move(payload)), signature(std::move(signature)) {}

    std::string toString() const {
        auto encodeSegment = [](const std::string& segment) -> std::string {
            auto encoded = jwt::base::encode<base64url_nopad>(segment);

            while (!encoded.empty() && encoded.back() == '=') {
                encoded.pop_back();
            }

            return encoded;
        };

        return encodeSegment(header) + "." + encodeSegment(payload) + "." + encodeSegment(signature);
    }
};