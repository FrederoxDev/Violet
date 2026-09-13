#pragma once
#include <openssl/evp.h>
#include <string>
#include <vector>
#include <expected>

class Base64 {
public:
    enum class Base64DecodeError {
        InvalidInput,
    };

    static std::expected<std::string, Base64DecodeError> decodeBase64(const std::string& input) {
        std::vector<unsigned char> buffer(
            (input.length() * 3) / 4 + 3
        );

        int len = EVP_DecodeBlock(
            buffer.data(),
            reinterpret_cast<const unsigned char*>(input.data()),
            input.length()
        );

        if (len < 0) {
            return std::unexpected(Base64DecodeError::InvalidInput);
        }

        if (!input.empty() && input.back() == '=') len--;
        if (input.size() > 1 && input[input.size() - 2] == '=') len--;

        return std::string(
            reinterpret_cast<const char*>(buffer.data()),
            len
        );
    }
};