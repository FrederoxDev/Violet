#pragma once
#include <jwt-cpp/base.h>

struct base64url_nopad : jwt::alphabet::base64url {
    static const std::string& fill() {
        static const std::string f{};
        return f;
    }
};